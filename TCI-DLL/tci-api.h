#pragma once

void AnnounceAll();
void AnnounceAll_old();

void BroadcastMessage(char * str);
void SendPlayerMessage(int playerID, char* str);
void ConsoleMessage(const char * str);

Vector3f GetPlayerPosition(int playerID);

int GetPlayerCount();

bool SpawnCar(char* carType, float x, float y, float z);
void SetDay();

void SpawnPlayerItem(int playerID, const char* item, int quantity, bool inHand);
