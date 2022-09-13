// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <thread>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <mutex>

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

BYTE* DLL_BRIDGE = NULL;

int * DLL_DETOURED;
int * DLL_IS_MAGIC_CALL;
int * DLL_COMMAND;
int * DLL_TRIGGER_DEBUG_CALL;

int * DLL_INTN_IN;
int * DLL_INTN_OUT;

int * DLL_INTS_IN; // array
int * DLL_INTS_OUT; // array

int * DLL_FLOATN_IN;
int * DLL_FLOATN_OUT;

float * DLL_FLOATS_IN; // array
float * DLL_FLOATS_OUT; // array

int * DLL_STRN_IN;
int * DLL_STRN_OUT;
int * DLL_STRLEN_IN; // array
int * DLL_STRLEN_OUT; // array


char** DLL_STRING_LOC = NULL;
char* DLL_STRING = NULL;

char** DLL_IN_STR1 = NULL;
char** DLL_IN_STR2 = NULL;
char** DLL_IN_STR3 = NULL;
char** DLL_IN_STR4 = NULL;

extern std::thread* LUA_THREAD;
void freeLuaStateInternally();
std::thread* setup(const char * path);
void cleanup();
void goInLua();

void LuaexecuteLine(const char* s);

std::ofstream MyOutputFile("C:\\Users\\Antonio\\Desktop\\DLL-Log-debugging.txt");


void AnnounceAll2() {
    MyOutputFile << "annoncer2\n" << std::flush;
    //DWORD* bridge = (DWORD*)DLL_BRIDGE;
    //bridge[6] = 1002;
    *DLL_COMMAND = 1002;
}

int l_AnnounceAll2(lua_State* L) {
    AnnounceAll2();

    return 0;
}

bool MockThreadSupport = false;
std::mutex mockMutex;
std::condition_variable mockcv;

void simpleThread() {

    MockThreadSupport = true;
    //mockcv.notify_one();


    // QUESTION : Does THIS thread crash if put to sleep with a cv? ( very important )
    // tested : it does not crash with a cv.wait.
}

int ESCALL_COMMAND;
int watermark7 = 0;

bool preserved = false;

// Preserving is very important and must be done right, if not, the server will jam in the iterationCycle
// or potentially crash ( bcs of runtime errors )
void pushESCall() {
    //DWORD* bridge = (DWORD*)DLL_BRIDGE;
    ESCALL_COMMAND = *DLL_COMMAND;
    watermark7 = DLL_STRLEN_OUT[0]; // ex bridge[7]
    preserved = true;
}

void popESCall() {
    //DWORD* bridge = (DWORD*)DLL_BRIDGE;
    *DLL_COMMAND = ESCALL_COMMAND;
    DLL_STRLEN_OUT[0] = watermark7;
    preserved = false;
}

void debugCall() {
    //DWORD* bridge = (DWORD*)DLL_BRIDGE;
    *DLL_TRIGGER_DEBUG_CALL = 12;
}

bool initializedLua = false;
bool SHOULD_LUA_HOTLOAD = false;

bool ESFirstCall = true; // signifies that OnGetControl is the first passthrough in an ES magic call cycle




// HERE IS AN IDEA :
// Instead of wasting time on call preserving ....
// Allow hot reloads only on update events - that will get ignored anyway?


volatile void OnGetControl() {

    //MyOutputFile << "Something" << std::flush;

    if (DLL_BRIDGE == NULL) {
        return;
    }

    //DWORD* bridge = (DWORD*)DLL_BRIDGE;

    if (*DLL_IS_MAGIC_CALL != 54) {
        return;
    }

    // TODO(UpdatePass+LuaHotReload) : Need a more homogenous way of running setup()
    // not just on 2103 .... + there should be a global bool flag indicating that it should be hot-reloaded.
    // SHOULD_LUA_RELOAD == true .. something like this
    // Also, there needs to be a way to call setup() again.. but dispose the old state cleanly ( For hot reloads )

    MyOutputFile << "FX0\n" << std::flush;

    //if (SHOULD_LUA_HOTLOAD && bridge[6] == 0 && initializedLua)
    if (SHOULD_LUA_HOTLOAD && ESFirstCall && initializedLua) { // if lua needs to hotload and we're not in the middle of something..
        freeLuaStateInternally();
        initializedLua = false;
        SHOULD_LUA_HOTLOAD = false;
        MyOutputFile << "FX-HR\n" << std::flush;
    }

    if (!initializedLua) {
        MyOutputFile << "FX-notInitialized\n" << std::flush;

        // TODO: do we really need the bridge[6] temp swap ?
        // are we sure we can't just let it do whatever it wants to do in setup()?
        // ..... (should investigate for readability reasons )
        //int tempCode = bridge[6];
        // Might wanna :
        // 1) preserve all the call details from enfusion script, and execute them AFTER the lua setup...
        // or
        // 2) Dedicate a separate call from EnfusionScript that is specifically made for LuaSetup
        //      -> however, a system needs to ensure that ES calls that lua setup after we are injected(detouring), and only once
        //      -> when we want to HOT Reload we need to call ES to time a luasetupcall for later

        pushESCall();

        //bridge[6] = 0; // change this with non-magic code ( from enum DayzServerCommands )
        //pushESCallContext();   // to be implemented


        LUA_THREAD = setup("C:\\Program Files (x86)\\Steam\\steamapps\\common\\DayZServer\\lua\\script.lua");

        // CURRENT STATUS :
        // If we call three broadcasts in a row, first one will be ignored because it is overwritten below ( bridge[6] = tempCode )
        // We need to delay the popESCallContext() until the Lua Thread places a 0 in bridge[6]

        initializedLua = true;

        //popESCallContext();   // to be implemented
        //bridge[6] = tempCode;

        

    }
    else {
        MyOutputFile << "FX-else\n" << std::flush;
        goInLua();
        // should try one more time to remove spinlock from main thread????? - tried, doesn't work
        // also test with OTHER mutex and cv. - tried, didn't work

        if (initializedLua && ((* DLL_COMMAND) == 0)) {
            MyOutputFile << "FX-ending\n" << std::flush;
            if (preserved) {
                MyOutputFile << "FX-popES\n" << std::flush;
                popESCall();
                goInLua(); // WARNING! THIS goInLua is immediately after another one. Sometimes the cv.notify() is too fast and
                // the threads jam! Be advised. - CONFIRMED CAUSE OF PROBLEMS. needs to be fixed! - somehow it might work under some conditions
                // but it is KNOWN to cause jams.
            }
        }
    }

    ESFirstCall = (* DLL_COMMAND) == 0;


    //MyOutputFile << "DDD\n" << std::flush;



    //MyOutputFile << "FFF\n" << std::flush;
    MyOutputFile << "FX-finale\n" << std::flush;


    /*
    if (bridge[6] == 2103) {
        MockThreadSupport = false;
        MyOutputFile << "pre-thread\n" << std::flush;
        std::thread newFunnyThread(simpleThread);
        //newFunnyThread.join(); // If we don't join the thread, it crashes the game
        newFunnyThread.detach(); // Detaching or joining works.

        MyOutputFile << "post-thread - wait stage\n" << std::flush;

        //std::unique_lock<std::mutex> lk(mockMutex);
        //MyOutputFile << "setup6\n" << std::flush;
        // ^^^ LAST THING TO BE PRINTED
        //mockcv.wait(lk, [] {return (MockThreadSupport == true); });

        //mockcv.wait(lk); // <--- the wait crashes the execution

        while(MockThreadSupport==false){//busy waiting - spin lock
        }

        MyOutputFile << "post-thread - post wait\n" << std::flush;
    }
    */


    
    // CURRENT PROBLEM *LATEST* : chekc if CV's work in threads .. and so on ..
    /*
    if (LUA_THREAD == NULL) {
        MyOutputFile << "Lua thread first time setup from server control\n" << std::flush;
        LUA_THREAD = setup(); // <--- as this doesn't work, THE LAUNCHING OF A THREAD SHOULD BE TESTED.
        //LUA_THREAD->detach();
        MyOutputFile << "Lua thread first time setup done\n" << std::flush;
    }
    */
    

    /*
    if (bridge[6] == 2103) {
        // CURRENT STATUS PROBLEM
        // This breaks execution :
        //LuaexecuteLine("function smecherie() local a = 10; a = a + 10; return a; end local b = smecherie()");
        //It might be because the Lua has to be invoked from another DLL - possible solution - try to compile as included lib...
        // Another TEST solution - inject the Lua.DLL BEFORE injecting the TCI dll // Tested - Not working


        MyOutputFile << "/lua executed\n" << std::flush;

        lua_State* newLuaState = luaL_newstate(); // <- this is where it crashes. apparently it's all about running code from other DLL
        MyOutputFile << "made new state\n" << std::flush;

        //luaL_openlibs(newLuaState); // <-- this crashed, probably because of the I/O lib trying to open stdout stdin stderr
        //MyOutputFile << "opened libs\n" << std::flush;

        luaopen_math(newLuaState); // <-- this worked
        MyOutputFile << "math lib\n" << std::flush; 

        lua_pushcfunction(newLuaState, l_AnnounceAll2);
        lua_setglobal(newLuaState, "annoncer");

        MyOutputFile << "annoncer - registered\n" << std::flush;

        bridge[6] = 0;

        int result = luaL_loadstring(newLuaState, "a = 10; annoncer();");
        MyOutputFile << "loadstring worked\n" << std::flush;

        switch (result) {
        case LUA_ERRSYNTAX:
            //puts("[Lua] Error executing line ( syntax ) !");
            break;
        case LUA_ERRMEM:
            //puts("[Lua] Error executing line ( memory ) !");
            break;
        default: {
            int res = lua_pcall(newLuaState, 0, LUA_MULTRET, 0);
            if (res != LUA_OK) {
                //print_error(state);
                return;
            }
        }
        }

        //https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking?redirectedfrom=MSDN
        //bridge[6] = 1002;
    }
    else {
        bridge[6] = 0;
    }
    */

    /*
    MyOutputFile << "before goInLua\n" << std::flush;
    // Current status problem : execution jams when calling /lua
    goInLua();
    MyOutputFile << "after goInLua\n" << std::flush;
    */

    /*
    if (bridge[6] == 2103) {
        bridge[6] = 1002;
    }
    else {
        bridge[6] = 0;
    }
    */
    
    
    //goInLua();
    


}
  
// CURRENT STATUS TODO :
// OnGetControl - place a fout call in it, with a flush, see if it enters the function ... etc
// in Detour, make sure the RETOUR is written before the DETOUR ( so the IP doesn't fly away in NOP's )
bool Detour(void* toHook, void* ourFunct, int len) {
    if (len < 14) {
        return false;
    }

    //MyOutputFile << "Detour 1\n" << std::flush;

    DWORD curProtection;
    VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

    //MyOutputFile << "Detour 2\n" << std::flush;

    char stolenBytes[128];

    memcpy(stolenBytes, toHook, len);
    memset(toHook, 0x90, len);

    //MyOutputFile << "Detour 3\n" << std::flush;

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

    //MyOutputFile << "Detour 4\n" << std::flush;

    // Retour

    //

    BYTE pushing[] = { 0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x55, 0x54, 0x41, 0x50, 0x41, 0x51, 0x41, 0x52, 0x41, 0x53, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x9C };
    
    BYTE popping[] = { 0x9D, 0x41, 0x5F, 0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C, 0x41, 0x5B, 0x41, 0x5A, 0x41, 0x59, 0x41, 0x58, 0x5C, 0x5D, 0x5F, 0x5E, 0x5A, 0x59, 0x5B, 0x58 };


    //MyOutputFile << "Detour 5\n" << std::flush;

    BYTE writingBuffer[1024];

    BYTE call[] = { 0xE8, 0x12, 0x23, 0x34, 0x45 };

    int index = 0;

    memcpy(writingBuffer + index, pushing, sizeof(pushing));
    index += sizeof(pushing);

    
    //MyOutputFile << "Detour 6\n" << std::flush;

    memcpy(writingBuffer + index, call, 5);
    index += 5;

    //MyOutputFile << "Detour 7\n" << std::flush;

    uint64_t here = (uint64_t)ourFunct + (uint64_t)index - (uint64_t)5;

    DWORD OnGetControlAdressJump = *((DWORD*)((BYTE*)OnGetControl + 1)); // jump to the actual func code
    uint64_t there = (uint64_t)OnGetControl + (uint64_t)OnGetControlAdressJump + 5;

    
    uint64_t relJump = there-here-5;
    DWORD relJump32 = relJump;

    *((DWORD*)(writingBuffer + index - 5 + 1)) = relJump32;
    

    memcpy(writingBuffer + index, popping, sizeof(popping));
    index += sizeof(popping);
    
    //MyOutputFile << "Detour 8\n" << std::flush;

    char dayzStolenBytes[] = { 0x49, 0x8B, 0x8E, 0xB0, 0x01, 0x00, 0x00, 0x48, 0x39, 0x01 };
    // NOTE : There was a lea instruction that doesn't get rewritten in the retour function
    // because the address used by it is relative so it wouldn't copy well over.
    // and it is 32 bit so it wouldn't work anyway ( because the jump is over too big of a distance )

    //memset(ourFunct, 0x90, retourLen);
    //memcpy(ourFunct, dayzStolenBytes, dzStolenBytesCount);

    memcpy(writingBuffer + index, dayzStolenBytes, sizeof(dayzStolenBytes));
    index += sizeof(dayzStolenBytes);

    //MyOutputFile << "Detour 9\n" << std::flush;

    // ourFunctRetour
    //void* ofRetour = (BYTE*)ourFunct + dzStolenBytesCount;
    void* ofRetour = writingBuffer + index;

    *(BYTE*)ofRetour = 0xFF;
    *(((BYTE*)ofRetour) + 1) = 0x25;
    *(((BYTE*)ofRetour) + 2) = 0x0;
    *(((BYTE*)ofRetour) + 3) = 0x0;
    *(((BYTE*)ofRetour) + 4) = 0x0;
    *(((BYTE*)ofRetour) + 5) = 0x0;
    *(uint64_t*)((uint64_t)ofRetour + 6) = (uint64_t)toHook + 14;
    index += 14;


    //MyOutputFile << "Detour 10\n" << std::flush;

    VirtualProtect(ourFunct, index, PAGE_EXECUTE_READWRITE, &curProtection);
    //MyOutputFile << "Detour 10.5\n" << std::flush;
    // Crashes here :
    memcpy(ourFunct, writingBuffer, index);

    //MyOutputFile << "Detour 11\n" << std::flush;

    VirtualProtect(ourFunct, index, curProtection, &temp);

    //MyOutputFile << "Detour 12\n" << std::flush;

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

    OnGetControl();
    // TCI-DLL.ourFunct+9D - E8 1A E6 FF FF           - call TCI-DLL.dll+112BC
    // TCI - DLL.dll + 112BC - E9 7F 17 00 00         - jmp TCI - DLL.OnGetControl
    // push rbp
    // push rdi
    // {intermediate calls}
    // pop rdi
    // pop rbp

    std::cout << "Some more junk" << std::endl;
}

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




struct TTT {
    unsigned int low, high;
};

union TConv {
    TTT t;
    unsigned long long int big;
};

const char * bridgeFile = "C:\\Users\\Antonio\\AppData\\Local\\DayZ\\testfile.txt";

void SetupDLLBridge() {

    std::ifstream MyReadFile(bridgeFile);
            ////std::ifstream MyReadFile("C:\\Users\\Antonio\\Desktop\\VictimWithFile\\testfile.txt");

    unsigned int temp;

    MyReadFile >> temp;

    MyReadFile.close();

    TConv t;
    t.big = 0;
    t.t.low = temp;


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
                break;
            }
        }

        t.t.high++;
    }

    //MyOutputFile << "-END OF FUNC-" << (long long)DLL_BRIDGE << std::flush;
    //                                     ^^^^^^ this was necessary

    //MyOutputFile.close();

    BYTE* ptr = DLL_BRIDGE + 4*sizeof(int);

    DLL_DETOURED = (int*)ptr; ptr += sizeof(int);
    DLL_IS_MAGIC_CALL = (int*)ptr; ptr += sizeof(int);
    DLL_COMMAND = (int*)ptr; ptr += sizeof(int);
    DLL_TRIGGER_DEBUG_CALL = (int*)ptr; ptr += sizeof(int);

    DLL_INTN_IN = (int*)ptr; ptr += sizeof(int);
    DLL_INTN_OUT = (int*)ptr; ptr += sizeof(int);

    //ptr += 4; // QWORD alignment

    ptr += 8; // ES Array header
    DLL_INTS_IN = (int*)ptr; ptr += 8 * sizeof(int);

    ptr += 8; // ES Array header
    DLL_INTS_OUT = (int*)ptr; ptr += 8 * sizeof(int);

    DLL_FLOATN_IN = (int*)ptr; ptr += sizeof(int);
    DLL_FLOATN_OUT = (int*)ptr; ptr += sizeof(int);

    ptr += 8; // ES Array header
    DLL_FLOATS_IN = (float*)ptr; ptr += 8 * sizeof(float);

    ptr += 8; // ES Array header
    DLL_FLOATS_OUT = (float*)ptr; ptr += 8 * sizeof(float);

    DLL_STRN_IN = (int*)ptr; ptr += sizeof(int);
    DLL_STRN_OUT = (int*)ptr; ptr += sizeof(int);

    ptr += 8; // ES Array header
    DLL_STRLEN_IN = (int*)ptr; ptr += 4 * sizeof(int);

    ptr += 8; // ES Array header
    DLL_STRLEN_OUT = (int*)ptr; ptr += 4 * sizeof(int);

    //Setting up the DLL_STRING bridge
    DLL_STRING_LOC = (char**)ptr; ptr += 8;
    DLL_STRING = *DLL_STRING_LOC;

    DLL_IN_STR1 = (char**)ptr; ptr += 8;
    DLL_IN_STR2 = (char**)ptr; ptr += 8;
    DLL_IN_STR3 = (char**)ptr; ptr += 8;
    DLL_IN_STR4 = (char**)ptr; ptr += 8;

    // TODO : Sometimes, there has been a spacer (8 bytes) observed between STRLEN_OUT and DLL_STRING_LOC in memory...

    strcpy(DLL_STRING, "Ok this is an interesting string");
}



inline bool file_exists_test(const std::string& name) {
    if (FILE* file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

DWORD WINAPI HackThread(HMODULE hModule) {
    //std::cout << "Hello World from DLL!" << std::endl;
    //std::cout << ourFunct << std::endl;

    while (!file_exists_test(bridgeFile)) { // maybe break after some time if this doesn't work.......
        Sleep(100);
    }

    SetupDLLBridge();

    //LUA_THREAD = setup();

    DWORD hookAddressJump = *((DWORD*)((BYTE*)ourFunct +1));
    uint64_t ourFunctLocation = (uint64_t)ourFunct + (uint64_t)hookAddressJump + 5;

    //DayZServer_x64.exe+2A8130
    const uint64_t relativeAddrFuncA = 0x2A8130;
    uint64_t hookAddress = (uint64_t)GetModuleHandle(NULL) + relativeAddrFuncA;


    MyOutputFile << "Hello - before detour\n" << std::flush;

    MyOutputFile << "-- " << (long long)ourFunct << '\n'  << std::flush;
    MyOutputFile << "-- " << hookAddressJump << '\n' << std::flush;
    MyOutputFile << "-- " << ourFunctLocation << '\n' << std::flush;

    //17 bytes
    Detour((void*)hookAddress, (void*)ourFunctLocation, 17);
    //Detour((void*)hookAddress, (void*)ourFunct, 17);
    // NOTE : Maybe it would be better to detour a DayZ Server VM operation of
    // assigning a value to an array member, because the instruction count might be smaller
    // than function calls - WHICH ARE A LOT

    // Note - when compiling with Lua Static instead of Lua DLL, having Detour being called here crashes the injection
    // (without calling the lua setup() and without calling any lua on OnGetControl)
    // If we don't call it.. the server doesn't crash
    // THE CULPRIT : if we compile with static lua instead of the dll version, the compiler somehow no longer makes that
    // ourFunct -> jmp -> actual code jump. Instead it goes directly into the ourFunct code ( to be overwritten )
    // TODO : Write some code that auto-detects if the function location reported by c++ is the real thing or a jump
    // and act accordingly.

    MyOutputFile << "Hello - post detour\n" << std::flush;


    if (remove(bridgeFile) != 0)
        MyOutputFile << "bridge file successfully deleted\n" << std::flush;
    else
        MyOutputFile << "bridge file couldn't be deleted\n" << std::flush;

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








#include "gui.h"

DWORD WINAPI GuiThread(HMODULE hModule) {
    gui::CreateHWindow("lsalal", "slalal Menu Class");
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