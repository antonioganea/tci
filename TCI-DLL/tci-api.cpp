#include "tci-api.h"


#include <Windows.h>
#include <iostream>
#include <fstream>

#include "dll-lua.h"
#include "gui.h"

#include "bridge.h"

//extern BYTE* DLL_BRIDGE;
#ifdef DESKTOP_DEBUG_FILE
extern std::ofstream MyOutputFile;
#endif

extern char** DLL_STRING_LOC;
extern char* DLL_STRING;
extern char** DLL_STRING2_LOC;

char workBuffer[1024];

union LongIntConverter {
    long long lng;
    int lo, hi;
};

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

int GetPlayerCount() {
    *DLL_COMMAND = 1010;

    goOutOfLua();

    return DLL_INTS_OUT[0];
}

int GetPlayerBySteamID(long long steamID) {
    *DLL_COMMAND = 1012;

    snprintf(DLL_STRING, 256, "%lld", steamID); // seems to be null-terminated.

    // Alternatively,
    // this is only a 'long' to str converter, not long long :
    //_ltoa(steamID, buffer, 10);

    DLL_STRLEN_IN[0] = strlen(DLL_STRING);

    goOutOfLua();

    return DLL_INTS_OUT[0];
}

long long GetPlayerSteamID(int playerID) {
    *DLL_COMMAND = 1014;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();

    char* steamIDstr = *DLL_IN_STR1;

    return strtoll(steamIDstr, NULL, 10);
}


float GetPlayerHealth(int playerID) {
    *DLL_COMMAND = 1015;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();

    return DLL_FLOATS_OUT[0];
}
float GetPlayerMaxHealth(int playerID) {
    *DLL_COMMAND = 1016;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();

    return DLL_FLOATS_OUT[0];
}
void SetPlayerHealth(int playerID, float value) {
    *DLL_COMMAND = 1017;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    DLL_FLOATS_IN[0] = value;
    *DLL_FLOATN_IN = 1;

    goOutOfLua();
}


float GetPlayerBlood(int playerID) {
    *DLL_COMMAND = 1018;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();

    return DLL_FLOATS_OUT[0];
}
float GetPlayerMaxBlood(int playerID) {
    *DLL_COMMAND = 1019;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();

    return DLL_FLOATS_OUT[0];
}
void SetPlayerBlood(int playerID, float value) {
    *DLL_COMMAND = 1020;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    DLL_FLOATS_IN[0] = value;
    *DLL_FLOATN_IN = 1;

    goOutOfLua();
}


float GetPlayerShock(int playerID) {
    *DLL_COMMAND = 1021;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();

    return DLL_FLOATS_OUT[0];
}
float GetPlayerMaxShock(int playerID) {
    *DLL_COMMAND = 1022;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();

    return DLL_FLOATS_OUT[0];
}
void SetPlayerShock(int playerID, float value) {
    *DLL_COMMAND = 1023;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    DLL_FLOATS_IN[0] = value;
    *DLL_FLOATN_IN = 1;

    goOutOfLua();
}


void KillPlayer(int playerID) {
    *DLL_COMMAND = 1024;

    DLL_INTS_IN[0] = playerID;
    *DLL_INTN_IN = 1;

    goOutOfLua();
}

int SpawnCar(char* carType, float x, float y, float z) {
    strcpy(DLL_STRING, carType);

    DLL_STRLEN_IN[0] = strlen(carType);
    *DLL_COMMAND = 1008;

    DLL_FLOATS_IN[0] = x;
    DLL_FLOATS_IN[1] = y;
    DLL_FLOATS_IN[2] = z;

    *DLL_FLOATN_IN = 3;

    goOutOfLua();

    return DLL_INTS_OUT[0];
}

int GetPlayerCar(int playerID) {
    *DLL_COMMAND = 1009;
    DLL_INTS_IN[0] = playerID;

    goOutOfLua();

    return DLL_INTS_OUT[0];
}

float GetCarFuel(int car) {
    *DLL_COMMAND = 1025;

    DLL_INTS_IN[0] = car;

    goOutOfLua();

    return DLL_FLOATS_OUT[0];
}

void SetCarFuel(int car, float fuel) {
    *DLL_COMMAND = 1026;

    DLL_INTS_IN[0] = car;

    DLL_FLOATS_IN[0] = fuel;

    goOutOfLua();
}

float GetCarFuelCapacity(int car) {
    *DLL_COMMAND = 1027;

    DLL_INTS_IN[0] = car;

    goOutOfLua();

    return DLL_FLOATS_OUT[0];
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

