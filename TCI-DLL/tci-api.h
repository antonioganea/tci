#pragma once

void AnnounceAll();
void AnnounceAll_old();

void BroadcastMessage(char * str);
void SendPlayerMessage(int playerID, char* str);
void ConsoleMessage(const char * str);

Vector3f GetPlayerPosition(int playerID);

int GetPlayerCount();
int GetPlayerBySteamID(long long steamID);
long long GetPlayerSteamID(int playerID);


float GetPlayerHealth(int playerID);
float GetPlayerMaxHealth(int playerID);
void SetPlayerHealth(int playerID, float value);

float GetPlayerBlood(int playerID);
float GetPlayerMaxBlood(int playerID);
void SetPlayerBlood(int playerID, float value);

float GetPlayerShock(int playerID);
float GetPlayerMaxShock(int playerID);
void SetPlayerShock(int playerID, float value);

void KillPlayer(int playerID);

long long SpawnCar(char* carType, float x, float y, float z);
void SetDay();

void SpawnPlayerItem(int playerID, const char* item, int quantity, bool inHand);
