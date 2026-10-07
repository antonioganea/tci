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
"41 8b 4c 24 08 48 c1 e1 05 49 03 ca ?? ?? ?? ?? ?? "
"?? 49 8b d4 ?? ?? ?? ?? ?? 4c 8b 54 24 50 48 8b c8 "
"?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? "
"?? ?? ?? ?? 48 8b 54 24 ?? 48 85 c9 ?? ?? ?? ?? ?? "
"?? 41 8b 44 24 04 89 01";


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
    uint64_t HOOK_POINT = Pattern::PatternScanStartingAt(BRIDGE_HOOK, 100, "89 01");

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
        // This will happen just before mov [rcx], eax

        uintptr_t address = ctx.rcx;
        int number = *((int*)address);

        if (number == 262987437 || number == 262987439)
        {
            console.AddLog("Found mov [rcx], eax | rax=%p rcx=%p | number at address %p", ctx.rax, ctx.rcx, (uint64_t)number);

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
reconstructed (claude opus 5.5) :

        case OP_LOAD_IMM: {
            uint32_t *dst = (uint32_t *)VarAddr(module, ip[2], ip);
            if (dst)
                *dst = ip[1];
            break;
        }

decompiled (ghidra) :

      case 0xb:
        lVar29 = (ulonglong)puVar41[2] * 0x20 + lVar18;
        if (*(char *)(lVar29 + 0x1c) == '\0') {
          puVar15 = (uint *)(**(longlong **)(lVar29 + 8) +
                            (ulonglong)*(ushort *)(lVar29 + 0x1a) * 0x28);
        }
        else {
          puVar15 = (uint *)FUN_1402df890(lVar29,puVar41);
          lVar18 = lStack_a08;
        }
        lVar29 = lStack_a18;
        if (puVar15 != (uint *)0x0) {
          *puVar15 = puVar41[1];
        }
        break;

signature:

41 8b 4c   
24 08
48 c1 e1 05
49 03 ca   
?? ?? ?? ??
?? ??      
49 8b d4   
?? ?? ??   
?? ??
4c 8b 54   
24 50
48 8b c8   
?? ??      
           
?? ?? ?? ??
?? ?? ?? ??
?? ?? ?? ??
?? ?? ??   
?? ?? ?? ??
           
48 8b 54   
24 ??
48 85 c9   
?? ?? ??   
?? ?? ??
41 8b 44   
24 04
89 01      

source :

                             switchD_1402e02ef::caseD_b                      XREF[1]:     1402e02ef(j)  
       1402e171d 41 8b 4c        MOV        ECX,dword ptr [R12 + 0x8]
                 24 08
       1402e1722 48 c1 e1 05     SHL        RCX,0x5
       1402e1726 49 03 ca        ADD        RCX,R10
       1402e1729 80 79 1c 00     CMP        byte ptr [RCX + 0x1c],0x0
       1402e172d 74 12           JZ         LAB_1402e1741
       1402e172f 49 8b d4        MOV        RDX,R12
       1402e1732 e8 59 e1        CALL       FUN_1402df890                                    undefined FUN_1402df890()
                 ff ff
       1402e1737 4c 8b 54        MOV        R10,qword ptr [RSP + 0x50]
                 24 50
       1402e173c 48 8b c8        MOV        RCX,RAX
       1402e173f eb 13           JMP        LAB_1402e1754
                             LAB_1402e1741                                   XREF[1]:     1402e172d(j)  
       1402e1741 0f b7 41 1a     MOVZX      EAX,word ptr [RCX + 0x1a]
       1402e1745 48 8d 14 80     LEA        RDX,[RAX + RAX*0x4]
       1402e1749 48 8b 41 08     MOV        RAX,qword ptr [RCX + 0x8]
       1402e174d 48 8b 08        MOV        RCX,qword ptr [RAX]
       1402e1750 48 8d 0c d1     LEA        RCX,[RCX + RDX*0x8]
                             LAB_1402e1754                                   XREF[1]:     1402e173f(j)  
       1402e1754 48 8b 54        MOV        RDX,qword ptr [RSP + 0x40]
                 24 40
       1402e1759 48 85 c9        TEST       RCX,RCX
       1402e175c 0f 84 51        JZ         switchD_1402e02ef::caseD_a
                 ed ff ff
       1402e1762 41 8b 44        MOV        EAX,dword ptr [R12 + 0x4]
                 24 04
       1402e1767 89 01           MOV        dword ptr [RCX],EAX                     <- RCX is the address, EAX is the data
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