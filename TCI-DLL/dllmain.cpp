// dllmain.cpp : Defines the entry point for the DLL application.

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <thread>

#include "bridge.h"
#include "detour.h"
#include "utils.h"

DWORD WINAPI HackThread(HMODULE hModule) {

    SetupDataBridge(); // blocks thread until bridge file is available for reading
    CreateDetour();

    return 0;
}

#include "gui.h"

DWORD WINAPI GuiThread(HMODULE hModule) {
    gui::CreateHWindow("TCI", "TCI Menu Class");
    gui::CreateDevice();
    gui::CreateImGui();

    while (gui::exit) {

        gui::BeginRender();
        gui::Render();
        gui::EndRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }

    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)GuiThread, hModule, 0, nullptr));
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        // Unsure if this is needed.
        //cleanup();
        break;
    }
    return TRUE;
}







// ATTIC :


/*
struct TTT {
    unsigned int low, high;
};

union TConv {
    TTT t;
    unsigned long long int big;
};

BYTE* DLL_BRIDGE = NULL;

void SetupDLLBridge() {
    std::ifstream MyReadFile("C:\\Users\\Antonio\\AppData\\Local\\DayZ\\testfile.txt");

    unsigned int temp;

    MyReadFile >> temp;

    MyReadFile.close();

    TConv t;
    t.big = 0;
    t.t.low = temp;

    std::ofstream MyOutputFile("C:\\Users\\Antonio\\AppData\\Local\\DayZ\\testfile-response.txt");
    MyOutputFile << std::hex << temp << std::dec << std::endl;

    // 01 3B AB E1 B3 BC AF F2 E3 1B 26 98 73 72 BC AD
    BYTE watermark[] = { 0x01, 0x3B, 0xAB, 0xE1, 0xB3, 0xBC, 0xAF, 0xF2, 0xE3, 0x1B, 0x26, 0x98, 0x73, 0x72, 0xBC, 0xAD };

    MyOutputFile << "X" << std::flush;

    MEMORY_BASIC_INFORMATION bas_inf[1024];

    //SYSTEM_INFO sysInfo;
    //GetSystemInfo(&sysInfo);
    //std::cout << "Page size : " << sysInfo.dwPageSize << std::endl;

    for (int i = 0; i < 1000; i++) {
        MyOutputFile << std::endl;

        MyOutputFile << "Y " << i << std::hex << t.big << std::dec << std::flush;
        DWORD returnVal = VirtualQuery(
            (LPCVOID)t.big,
            bas_inf,
            1024
        );
        MyOutputFile << "Z" << std::flush;

        // PAGE_NOACCESS 0x01
        // PAGE_READONLY 0x02
        // PAGE_READWRITE 0x04
        // ...
        // PAGE_EXECUTE_READWRITE 0x40

        //std::cout << returnVal << std::endl;
        //std::cout << bas_inf[0].Protect << std::endl;

        if (returnVal == 0) {
            continue;
        }
        MyOutputFile << "W " << returnVal << " " << std::hex << bas_inf[0].Protect << std::dec << " " << std::flush;

        if (bas_inf[0].Protect != PAGE_READWRITE) {
            continue;
        }
        MyOutputFile << "O" << std::flush;

        if (memcmp((void*)t.big, watermark, sizeof(watermark)) == 0) {
            DLL_BRIDGE = (BYTE*)t.big;
            MyOutputFile << "Worked. Hooked." << DLL_BRIDGE << std::flush;
            break;
        }
        MyOutputFile << "T";
        t.t.high++;
    }

    //DWORD procID = GetProcessId(GetCurrentProcess());
    //bool success = ReadProcessMemory(GetCurrentProcess(), (LPCVOID)100, &readingBuffer, 4, &BYTES_READ);

    MyOutputFile << "-END OF FUNC-" << (long long)DLL_BRIDGE << std::flush;
    //                                   ^^^^^^^ this was necessary

    MyOutputFile.close();
}
*/


bool DetourGeneric(void* toHook, void* ourFunct, int len) {
    if (len < 14) {
        return false;
    }

    DWORD curProtection;
    VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

    char stolenBytes[128];

    memcpy(stolenBytes, toHook, len);
    memset(toHook, 0x90, len);

    // Detour
    *(BYTE*)toHook = 0xFF;
    *(((BYTE*)toHook) + 1) = 0x25;
    *(((BYTE*)toHook) + 2) = 0x0;
    *(((BYTE*)toHook) + 3) = 0x0;
    *(((BYTE*)toHook) + 4) = 0x0;
    *(((BYTE*)toHook) + 5) = 0x0;
    *(uint64_t*)((uint64_t)toHook + 6) = (uint64_t)ourFunct;

    DWORD temp;
    VirtualProtect(toHook, len, curProtection, &temp);

    // Retour
    int retourLen = 14 + len;
    VirtualProtect(ourFunct, retourLen, PAGE_EXECUTE_READWRITE, &curProtection);

    memset(ourFunct, 0x90, retourLen);
    memcpy(ourFunct, stolenBytes, len);

    // ourFunctRetour
    void* ofRetour = (BYTE*)ourFunct + len;

    *(BYTE*)ofRetour = 0xFF;
    *(((BYTE*)ofRetour) + 1) = 0x25;
    *(((BYTE*)ofRetour) + 2) = 0x0;
    *(((BYTE*)ofRetour) + 3) = 0x0;
    *(((BYTE*)ofRetour) + 4) = 0x0;
    *(((BYTE*)ofRetour) + 5) = 0x0;
    *(uint64_t*)((uint64_t)ofRetour + 6) = (uint64_t)toHook + 14;

    VirtualProtect(ourFunct, retourLen, curProtection, &temp);

    return true;
}

// UNUSED
//DWORD WINAPI HackThreadInjectionVictim(HMODULE hModule) {
    /*
    int hookLength = 6;
    DWORD hookAddress = 0x332768;
    jmpBackAddy = hookAddress + hookLength;

    Hook((void*)hookAddress, ourFunct, hookLength);
    */

    // 7FF720A20000 - process base address
    // 7FF720A32770 - funcA
    // diff : 75632 dec, 0x12770 hex


    //char stolenBytes[15] = { 0x40, 0x55, 0x57, 0x48, 0x81, 0xEC, 0xE8, 0x00, 0x00, 0x00, 0x48, 0x8D, 0x6C, 0x24, 0x20 };
    //char jumpBytes[15] =   { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x10, 0x25, 0xB1, 0xB6, 0xFC, 0x7F, 0x00, 0x00, 0x90 };
    
    /*
    std::cout << "Hello World from DLL!" << std::endl;
    std::cout << ourFunct << std::endl;
    DWORD hookAddressJump = *((DWORD*)((BYTE*)ourFunct + 1));// *(DWORD*)(((void*)hookAddress) + 1);
    std::cout << "Hook Address Jump : " << hookAddressJump << std::endl;
    uint64_t ourFunctLocation = (uint64_t)ourFunct + (uint64_t)hookAddressJump + 5;
    std::cout << "Landing address : " << ourFunctLocation << std::endl;

    //std::cout << ((uint64_t)ourFunct - (uint64_t)hookAddress) - 6 << std::endl;
    std::cout << "DLL Base address : " << (uint64_t)hModule << std::endl;
    std::cout << "Process Base address : " << (uint64_t)GetModuleHandle(NULL) << std::endl;


    const uint64_t relativeAddrFuncA = 0x12770;
    uint64_t hookAddress = (uint64_t)GetModuleHandle(NULL) + relativeAddrFuncA;

    Detour((void*)hookAddress, (void*)ourFunctLocation, 15);

    return 0;
    */
//}


/*
DWORD WINAPI HackThread(HMODULE hModule)
{
    AllocConsole();
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
    moduleBase = (uintptr_t)GetModuleHandle(NULL);

    if (GetAsyncKeyState(VK_END) & 1) {}
    if (GetAsyncKeyState(VK_NUMPAD1) & 1) {}
    Sleep(5);

    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
}
*/

/*


void ThreadA_Activity()
{
    std::cout << "Thread A started " << std::endl;

    cv.notify_one();//notify to ThreadB that he can start doing his job

    // wait for the Thread B
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return false; });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "end of Thread A" << std::endl;
}

*/