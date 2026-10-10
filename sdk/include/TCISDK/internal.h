#pragma once

#define SDKAPI extern "C" __declspec(dllimport)
#define SDKAPI_STRUCT __declspec(dllimport)

namespace TCISDK_Internal {

struct SDKAPI_STRUCT Vector3f
{
    float x, y, z;
};

SDKAPI void LoadExtension(const char *extName);
SDKAPI void FreeExtension(const char *extName);
SDKAPI void CallExtension(const char *extName, const char *funcName, int argc, const char **argv);

SDKAPI void BroadcastMessage(const char * str);
SDKAPI void SendPlayerMessage(int playerID, const char* str);
SDKAPI void ConsoleMessage(const char * str);

SDKAPI Vector3f GetPlayerPosition(int playerID);
SDKAPI void SetPlayerPosition(int playerID, Vector3f pos);

SDKAPI int GetPlayerCount();
SDKAPI int GetPlayerBySteamID(long long steamID);
SDKAPI long long GetPlayerSteamID(int playerID);

SDKAPI float GetPlayerHealth(int playerID);
SDKAPI float GetPlayerMaxHealth(int playerID);
SDKAPI void SetPlayerHealth(int playerID, float value);

SDKAPI float GetPlayerBlood(int playerID);
SDKAPI float GetPlayerMaxBlood(int playerID);
SDKAPI void SetPlayerBlood(int playerID, float value);

SDKAPI float GetPlayerShock(int playerID);
SDKAPI float GetPlayerMaxShock(int playerID);
SDKAPI void SetPlayerShock(int playerID, float value);

SDKAPI void KillPlayer(int playerID);

SDKAPI int SpawnCar(char* carType, float x, float y, float z);
SDKAPI int GetPlayerCar(int playerID);
SDKAPI float GetCarFuel(int car);
SDKAPI void SetCarFuel(int car, float fuel);
SDKAPI float GetCarFuelCapacity(int car);

SDKAPI void SetDay();

SDKAPI void SpawnPlayerItem(int playerID, const char *item, int quantity, bool inHand);

}