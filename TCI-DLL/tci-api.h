#pragma once

void AnnounceAll();
void AnnounceAll_old();

void BroadcastMessage(char * str);
void ConsoleMessage(char * str);

//Call from Lua to Cpp
void SpawnOlga();
void SetDay();