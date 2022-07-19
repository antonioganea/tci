// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <iostream>
#include <Windows.h>

/*

40 55 57 48 81 EC E8 00 00 00 48 8D 6C 24 20

FF 25 00 00 00 00 10 25 B1 B6 FC 7F 00 00 90

1  2  3  4  5  6  7  8  9  10 11 12 13 14 15

relative jmp
jmp qword ptr [injectionVictim.funcB]
FF 25 4A 00 00 00

4A - 50


// func a - base diff
7fffa27a24b0
7ff720a20000

*/



/*

Register preservation instructions:
( can be optimized for sure )

50 53 51 52 56 57 55 54 41 50 41 51 41 52 41 53 41 54 41 55 41 56 41 57 9C //pushing
90 // nop
9D 41 5F 41 5E 41 5D 41 5C 41 5B 41 5A 41 59 41 58 5C 5D 5F 5E 5A 59 5B 58 //popping

^ Might be incorrect - pushfq acts on "eflags" ... maybe it doesn't include the normal flags - needs googling

*/


  

bool Detour(void* toHook, void* ourFunct, int len) {
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
    const int dzStolenBytesCount = 10;
    int retourLen = 14 + dzStolenBytesCount;
    VirtualProtect(ourFunct, retourLen, PAGE_EXECUTE_READWRITE, &curProtection);

    char dayzStolenBytes[dzStolenBytesCount] = { 0x49, 0x8B, 0x8E, 0xB0, 0x01, 0x00, 0x00, 0x48, 0x39, 0x01 };

    memset(ourFunct, 0x90, retourLen);
    memcpy(ourFunct, dayzStolenBytes, dzStolenBytesCount);

    // ourFunctRetour
    void* ofRetour = (BYTE*)ourFunct + dzStolenBytesCount;

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
    *(uint64_t*)((uint64_t)ofRetour + 6) = (uint64_t)toHook+14;

    VirtualProtect(ourFunct, retourLen, curProtection, &temp);

    return true;
}

// dummy function
volatile void ourFunct() {
    std::cout << "DLL ourfunct" << std::endl;
    std::cout << "extra junk" << std::endl;

    for (int i = 0; i < 5; i++) {
        std::cout << "Extra spam in a for loop" << std::endl;
    }
}

volatile void OnGetControl() {
    
    // ...

}

DWORD WINAPI HackThread(HMODULE hModule) {
    //std::cout << "Hello World from DLL!" << std::endl;
    //std::cout << ourFunct << std::endl;
    DWORD hookAddressJump = *((DWORD*)((BYTE*)ourFunct +1));
    uint64_t ourFunctLocation = (uint64_t)ourFunct + (uint64_t)hookAddressJump + 5;

    //DayZServer_x64.exe+2A8130
    const uint64_t relativeAddrFuncA = 0x2A8130;
    uint64_t hookAddress = (uint64_t)GetModuleHandle(NULL) + relativeAddrFuncA;

    //17 bytes
    Detour((void*)hookAddress, (void*)ourFunctLocation, 17);
    
    return 0;
}


DWORD WINAPI HackThreadInjectionVictim(HMODULE hModule) {
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
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}










/*
#include "stdafx.h"
#include <iostream>
#include "mem.h"

DWORD WINAPI HackThread(HMODULE hModule)
{
    //Create Console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "OG for a fee, stay sippin' fam\n";

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    //calling it with NULL also gives you the address of the .exe module
    moduleBase = (uintptr_t)GetModuleHandle(NULL);

    bool bHealth = false, bAmmo = false, bRecoil = false;

    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }

        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
            bHealth = !bHealth;

        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bAmmo = !bAmmo;
        }

        //no recoil NOP
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
            }

            else
            {
                //50 8D 4C 24 1C 51 8B CE FF D2 the original stack setup and call
                mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
            }
        }

        //need to use uintptr_t for pointer arithmetic later
        uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);

        //continuous writes / freeze

        if (localPlayerPtr)
        {
            if (bHealth)
            {

                //*localPlayerPtr = derference the pointer, to get the localPlayerAddr
                // add 0xF8 to get health address
                //cast to an int pointer, this pointer now points to the health address
                //derference it and assign the value 1337 to the health variable it points to
                *(int*)(*localPlayerPtr + 0xF8) = 1337;
            }

            if (bAmmo)
            {
                //We aren't external now, we can now efficiently calculate all pointers dynamically
                //before we only resolved pointers when needed for efficiency reasons
                //we are executing internally, we can calculate everything when needed
                uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 });
                int* ammo = (int*)ammoAddr;
                *ammo = 1337;

                //or just
                *(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;
            }

        }
        Sleep(5);
    }

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

*/