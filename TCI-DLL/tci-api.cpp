#include "tci-api.h"
#include "pch.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

#include "dll-lua.h"
#include "gui.h"

extern BYTE* DLL_BRIDGE;
extern std::ofstream MyOutputFile;

extern char** DLL_STRING_LOC;
extern char* DLL_STRING;
extern char** DLL_STRING2_LOC;

char workBuffer[1024];

void AnnounceAll() {
    DWORD* bridge = (DWORD*)DLL_BRIDGE;

    //MyOutputFile << "AnnounceAll -internally " << (int)DLL_BRIDGE[7 * 4] << " " << DLL_BRIDGE[7 * 4 + 1] << DLL_BRIDGE[7 * 4 + 2] << "\n" << std::flush;
    //for (int i = 1; i <= DLL_BRIDGE[7 * 4]; i++) {
    //    MyOutputFile << DLL_BRIDGE[7 * 4 + i];
    //}
    //MyOutputFile << std::flush;

    memcpy(workBuffer, *DLL_STRING2_LOC, bridge[7]);


    memcpy(DLL_STRING, workBuffer, bridge[7]);
    memcpy(DLL_STRING + bridge[7], workBuffer, bridge[7]);

    bridge[7] *= 2;
    bridge[6] = 1002;

    goOutOfLua();
}

void BroadcastMessage(char* str) {
    DWORD* bridge = (DWORD*)DLL_BRIDGE;

    strcpy(DLL_STRING, str);

    bridge[7] = strlen(str);
    bridge[6] = 1002;

    goOutOfLua();
}


extern TCIAppConsole console;

void ConsoleMessage(const char* str) {
    console.AddLog(str);
}

void AnnounceAll_old() {
    DWORD* bridge = (DWORD*)DLL_BRIDGE;
    bridge[6] = 1002;
    MyOutputFile << "AnnounceAll -internally " << (int)DLL_BRIDGE[7 * 4] << " " << DLL_BRIDGE[7 * 4 + 1] << DLL_BRIDGE[7 * 4 + 2] << "\n" << std::flush;
    for (int i = 1; i <= DLL_BRIDGE[7 * 4]; i++) {
        MyOutputFile << DLL_BRIDGE[7 * 4 + i];
    }
    MyOutputFile << std::flush;
    goOutOfLua();
}

//Call from Lua to Cpp
void SpawnOlga() {
    std::cout << "Spawn Olga point" << std::endl;

    goOutOfLua();

    std::cout << "Spawn Olga done" << std::endl;

    // Writes to shared_mem (bridge)
    // NEEDS TO PASS CONTROL TO DayZServer_x64
    //giveControl();
}

void SetDay() {
    std::cout << "Set day point" << std::endl;

    goOutOfLua();

    /*
    cv.notify_one();
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return false; });
    }
    */

    std::cout << "Set day done" << std::endl;

    // Writes to shared_mem (bridge)
    // NEEDS TO PASS CONTROL TO DayZServer_x64
    //giveControl();
}
