#pragma once

void AnnounceAll();
void AnnounceAll_old();

void BroadcastMessage(char * str);

//Call from Lua to Cpp
void SpawnOlga();
void SetDay();