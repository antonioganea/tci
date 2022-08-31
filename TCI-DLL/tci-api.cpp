#include "tci-api.h"
#include "pch.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

#include "dll-lua.h"
#include "gui.h"

#include "bridge.h"

//extern BYTE* DLL_BRIDGE;
extern std::ofstream MyOutputFile;

extern char** DLL_STRING_LOC;
extern char* DLL_STRING;
extern char** DLL_STRING2_LOC;

char workBuffer[1024];

void AnnounceAll() {
    /*
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
    */
}

void BroadcastMessage(char* str) {
    //DWORD* bridge = (DWORD*)DLL_BRIDGE;

    strcpy(DLL_STRING, str);

    
    DLL_STRLEN_IN[0] = strlen(str);
    *DLL_COMMAND = 1002;

    goOutOfLua();
}

void SendPlayerMessage(int playerID, char* str) {
    strcpy(DLL_STRING, str);

    DLL_STRLEN_IN[0] = strlen(str);
    *DLL_COMMAND = 1006;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();
}


extern TCIAppConsole console;

void ConsoleMessage(const char* str) {
    console.AddLog(str);
}

Vector3f GetPlayerPosition(int playerID) {
    Vector3f pos;

    *DLL_COMMAND = 5681;
    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();

    pos.x = DLL_FLOATS_OUT[0];
    pos.y = DLL_FLOATS_OUT[1];
    pos.z = DLL_FLOATS_OUT[2];

    return pos;
}

void AnnounceAll_old() {
    /*
    DWORD* bridge = (DWORD*)DLL_BRIDGE;
    bridge[6] = 1002;
    MyOutputFile << "AnnounceAll -internally " << (int)DLL_BRIDGE[7 * 4] << " " << DLL_BRIDGE[7 * 4 + 1] << DLL_BRIDGE[7 * 4 + 2] << "\n" << std::flush;
    for (int i = 1; i <= DLL_BRIDGE[7 * 4]; i++) {
        MyOutputFile << DLL_BRIDGE[7 * 4 + i];
    }
    MyOutputFile << std::flush;
    goOutOfLua();
    */
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


void SpawnPlayerItem(int playerID, const char* item, int quantity, bool inHand) {
    
    *DLL_COMMAND = 5682;

    DLL_INTS_IN[0] = playerID;
    DLL_INTS_IN[1] = quantity;
    DLL_INTS_IN[2] = (int)inHand;
    *DLL_INTN_IN = 3;

    strcpy(DLL_STRING, item);
    DLL_STRLEN_IN[0] = strlen(item);

    goOutOfLua();
}

