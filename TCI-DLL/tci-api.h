#pragma once

void AnnounceAll();
void AnnounceAll_old();

void BroadcastMessage(char * str);
void ConsoleMessage(const char * str);

Vector3f GetPlayerPosition(int playerID);

//Call from Lua to Cpp
void SpawnOlga();
void SetDay();