#include "detour.h"

#include <Windows.h>
#include <cstdint>

#include <iostream>

#include "magic-call.h"

#define DEFERRED_JMP_FUNCTIONS

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

#ifdef DEFERRED_JMP_FUNCTIONS
    DWORD OnGetControlAdressJump = *((DWORD*)((BYTE*)OnGetControl + 1)); // jump to the actual func code
    uint64_t there = (uint64_t)OnGetControl + (uint64_t)OnGetControlAdressJump + 5;
#else
    uint64_t there = (uint64_t)OnGetControl;
#endif

    uint64_t relJump = there - here - 5;
    DWORD relJump32 = relJump;

    *((DWORD*)(writingBuffer + index - 5 + 1)) = relJump32;


    memcpy(writingBuffer + index, popping, sizeof(popping));
    index += sizeof(popping);

    //MyOutputFile << "Detour 8\n" << std::flush;

    char dayzStolenBytes[] = { 0x49, 0x8B, 0x8E, 0xB8, 0x01, 0x00, 0x00, 0x48, 0x39, 0x01 };
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




void CreateDetour() {

#ifdef DEFERRED_JMP_FUNCTIONS
    DWORD hookAddressJump = *((DWORD*)((BYTE*)ourFunct + 1));
    uint64_t ourFunctLocation = (uint64_t)ourFunct + (uint64_t)hookAddressJump + 5;
#else
    uint64_t ourFunctLocation = (uint64_t)ourFunct;
#endif

    //DayZServer_x64.exe+2A8130 <- DayZ 1.18
    //0x2A770B <- DayZ 1.19 first version
    //0x2A797B <- DayZ 1.20 (?)
    //0x2ABACB <- DayZ 1.20.155814
    //0x2AC36B <- DayZ 1.20.155844

    const uint64_t relativeAddrFuncA = 0x2AC36B;
    uint64_t hookAddress = (uint64_t)GetModuleHandle(NULL) + relativeAddrFuncA;

#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "Hello - before detour\n" << std::flush;

    MyOutputFile << "-- " << (long long)ourFunct << '\n' << std::flush;
    MyOutputFile << "-- " << hookAddressJump << '\n' << std::flush;
    MyOutputFile << "-- " << ourFunctLocation << '\n' << std::flush;
#endif

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

}