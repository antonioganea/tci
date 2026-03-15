#include "bridge.h"

#include <KnownFolders.h>
#include <shlobj.h>

#include "gui.h"

#include <fstream>
#include "pattern.h"

#include <safetyhook.hpp>
#include "detour.h"

SafetyMidHook bridge_hook;

std::string bridge_hook_signature = 
"49 8b 94 24 b0 00 00 00 48 8b 44 24 48 8b 40 08 48 "
"8d 0c 80 48 8d 0c ca e8 ?? ?? ?? ?? 48 8b 54 24 48 "
"4c 8d 0d ?? ?? ?? ?? 44 8b 44 24 40 45 33 db 48 85 "
"c0 0f 84 ?? ?? ?? ?? 8b 4a 04 89 08";


void SetupDataBridge()
{
    // This implementation uses a hook as a method to find the watermark faster.

    uint64_t BRIDGE_HOOK = Pattern::PatternScan(GetModuleHandle(NULL), bridge_hook_signature.c_str());

    if (BRIDGE_HOOK == 0)
    {
        console.AddLog("Couldn't find injection point for data bridge (signature scanning failed)\n");
        return;
    }

    console.AddLog("Found BRIDGE_HOOK at %p", BRIDGE_HOOK - (uint64_t)GetModuleHandle(NULL));

    // This finds the exact injection point for mov [rax], ecx
    uint64_t HOOK_POINT = Pattern::PatternScanStartingAt(BRIDGE_HOOK, 100, "89 08");

    if (HOOK_POINT == 0)
    {
        console.AddLog("Couldn't find hook point inside broader scanned signature");
        return;
    }

    console.AddLog("Found HOOK_POINT at %p", HOOK_POINT - (uint64_t)GetModuleHandle(NULL));

    console.AddLog("Found injection point for data bridge through signature scanning at %p\n",(HOOK_POINT-(uint64_t)GetModuleHandle(NULL)));

    console.AddLog("Creating data bridge hook ..!\n");

    bridge_hook = safetyhook::create_mid(HOOK_POINT, [] (safetyhook::Context& ctx)
    {
        // This will happen just before mov [rax], ecx

        uintptr_t address = ctx.rax;
        int number = *((int*)address);

        if (number == 262987437 || number == 262987439)
        {
            console.AddLog("Found mov [rax], ecx | rax=%p rcx=%p | number at address %p", address, ctx.rcx, (uint64_t)number);

            uint64_t WATERMARK_ADDRESS = Pattern::PatternScanStartingAt(address+1024, 200,
                "01 3B AB E1 B3 BC AF F2 E3 1B 26 98 73 72 BC AD");
            
            if (WATERMARK_ADDRESS == 0)
            {
                console.AddLog("Couldn't locate watermark after finding DLL_FLIP_NUMBER's location");

                std::thread t([]() { bridge_hook = {}; });
                t.detach();

                return;
            }

            console.AddLog("Found watermark address at %p", WATERMARK_ADDRESS);

            DLL_BRIDGE = (BYTE*)WATERMARK_ADDRESS;
            bindBridgeLayout();

            //bridge_hook = {};

            std::thread x([]() {
                bridge_hook = {};
                CreateDetour();
            });
            x.detach();
        }
    });
}

/*
switchD_1402c23e8::caseD_b                      XREF[1]:     1402c23e8(j)  
       1402c318b 49 8b 94        MOV        RDX,qword ptr [R12 + 0xb0]
                 24 b0 00 
                 00 00
       1402c3193 48 8b 44        MOV        RAX,qword ptr [RSP + local_610]
                 24 48
       1402c3198 8b 40 08        MOV        EAX,dword ptr [RAX + 0x8]
       1402c319b 48 8d 0c 80     LEA        RCX,[RAX + RAX*0x4]
       1402c319f 48 8d 0c ca     LEA        RCX,[RDX + RCX*0x8]
       1402c31a3 e8 28 e8        CALL       FUN_1402c19d0                                    undefined FUN_1402c19d0()
                 ff ff
       1402c31a8 48 8b 54        MOV        RDX,qword ptr [RSP + local_610]
                 24 48
       1402c31ad 4c 8d 0d        LEA        R9,[IMAGE_DOS_HEADER_140000000]
                 4c ce d3 ff
       1402c31b4 44 8b 44        MOV        R8D,dword ptr [RSP + local_618]
                 24 40
       1402c31b9 45 33 db        XOR        R11D,R11D
       1402c31bc 48 85 c0        TEST       RAX,RAX
       1402c31bf 0f 84 85        JZ         LAB_1402c254a
                 f3 ff ff
       1402c31c5 8b 4a 04        MOV        ECX,dword ptr [RDX + 0x4]
       1402c31c8 89 08           MOV        dword ptr [RAX],ECX                           <- RAX is the address, ECX is the data
*/

void SetupDataBridge_SLOW()
{
    while(true)
    {
        uint64_t bridgePoint =  Pattern::PatternScanAllMemory("01 3B AB E1 B3 BC AF F2 E3 1B 26 98 73 72 BC AD");

        if (bridgePoint != 0)
        {
            DLL_BRIDGE = (BYTE*)bridgePoint;
            console.AddLog("Scanned for watermark. Worked. Hooked. %p", DLL_BRIDGE);
            bindBridgeLayout();
            break;
        }
        else
        {
            console.AddLog("Scanned for bridge watermark. Not Found. Waiting 500ms and then trying again");
            Sleep(500);
        }
    }
}

//////////////// OLD IMPLEMENTATION BELOW ////////////////

// Returns something like : "C:\\Users\\Antonio\\AppData\\Local\\DayZ\\testfile.txt";
std::wstring GetBridgeFilePath() {
    std::wstring bridgeFile;

    PWSTR localAppDataPath;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppDataPath);

    bridgeFile = localAppDataPath;
    bridgeFile += L"\\DayZ\\testfile.txt";

    return bridgeFile;
}

unsigned int readBridgeFileValue(std::wstring filePath) {
    std::ifstream MyReadFile(filePath);
    unsigned int temp;
    MyReadFile >> temp;
    MyReadFile.close();
    return temp;
}

// TODO : return bool, and add error handling
void ScanForWatermark(unsigned int lowIntVal) {
    TConv t;
    t.big = 0;
    t.t.low = lowIntVal;

    ////std::ofstream MyOutputFile("C:\\Users\\Antonio\\Desktop\\VictimWithFile\\testfile-response.txt");
//std::ofstream MyOutputFile("C:\\Users\\Antonio\\AppData\\Local\\DayZ\\testfile-response.txt");
// MyOutputFile << std::hex << temp << std::dec << std::endl;


// 01 3B AB E1 B3 BC AF F2 E3 1B 26 98 73 72 BC AD
    BYTE watermark[] = { 0x01, 0x3B, 0xAB, 0xE1, 0xB3, 0xBC, 0xAF, 0xF2, 0xE3, 0x1B, 0x26, 0x98, 0x73, 0x72, 0xBC, 0xAD };

    //MyOutputFile << "X" << std::flush;

    BYTE readBuffer[1000];
    SIZE_T bytesRead;

    HANDLE currentProcess = GetCurrentProcess();

    for (unsigned int i = 0; i <= 0xFFFFFFFF; i++) { // unsigned int ??? - might not work 

        bool success = ReadProcessMemory(currentProcess, (LPCVOID)t.big, &readBuffer, sizeof(watermark), &bytesRead);

        if (success) {
            //MyOutputFile << " Success : ";
            if (memcmp((void*)t.big, watermark, sizeof(watermark)) == 0) {
                DLL_BRIDGE = (BYTE*)t.big;
                //MyOutputFile << "Worked. Hooked." << std::hex << (long long) DLL_BRIDGE << std::dec << std::flush;
                console.AddLog("Scanned for watermark. Worked. Hooked. %p\n", DLL_BRIDGE);
                break;
            }
        }

        t.t.high++;
    }

    //MyOutputFile << "-END OF FUNC-" << (long long)DLL_BRIDGE << std::flush;
    //                                     ^^^^^^ this was necessary

    //MyOutputFile.close();
}

// TODO : return bool, and add error handling in dllmain.cpp
void SetupDataBridge_File_Based() {
    std::wstring bridgeFile = GetBridgeFilePath();

    while (!fileExistsTest(bridgeFile)) { // maybe break after some time if this doesn't work.......
        Sleep(100);
    } // TODO : print to console when file was found...
    
    unsigned int lowInt = readBridgeFileValue(bridgeFile);

    // TODO : add error handling here
    ScanForWatermark(lowInt); // this sets DLL_BRIDGE correctly

    if (DLL_BRIDGE == NULL) {
        // TODO : error handling
    }

    bindBridgeLayout();

    if (_wremove(bridgeFile.c_str()) != 0) {
        // TODO: Add some console logs
    }
    else {
        // TODO: Add some console logs
    }
}