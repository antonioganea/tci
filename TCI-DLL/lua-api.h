#pragma once

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

// ConsoleMessage("this should appear in the console")
int l_ConsoleMessage(lua_State* L);

// BroadcastMessage("Our discord is discord.gg/something")
int l_BroadcastMessage(lua_State* L);

// SendPlayerMessage(myPlayer, "How do you do today?")
int l_SendPlayerMessage(lua_State* L);

// car = SpawnCar("olga", x, y, z)
int l_SpawnCar(lua_State* L);

// car = GetPlayerCar(playerID)
int l_GetPlayerCar(lua_State* L);

// playerCount = GetPlayerCount()
int l_GetPlayerCount(lua_State* L);

// player = GetPlayerBySteamID(steamID)
int l_GetPlayerBySteamID(lua_State* L);

// steamID = GetPlayerSteamID(myPlayer)
int l_GetPlayerSteamID(lua_State* L);

// health = GetPlayerHealth(myPlayer)
int l_GetPlayerHealth(lua_State* L);

// maxHealth = GetPlayerMaxHealth(myPlayer)
int l_GetPlayerMaxHealth(lua_State* L);

// SetPlayerHealth(myPlayer, health+10)
int l_SetPlayerHealth(lua_State* L);

// blood = GetPlayerBlood(myPlayer)
int l_GetPlayerBlood(lua_State* L);

// maxBlood = GetPlayerMaxBlood(myPlayer)
int l_GetPlayerMaxBlood(lua_State* L);

// SetPlayerBlood(myPlayer, blood+10)
int l_SetPlayerBlood(lua_State* L);

// shock = GetPlayerShock(myPlayer)
int l_GetPlayerShock(lua_State* L);

// maxShock = GetPlayerMaxShock(myPlayer)
int l_GetPlayerMaxShock(lua_State* L);

// SetPlayerShock(myPlayer, shock+10)
int l_SetPlayerShock(lua_State* L);

// KillPlayer(myPlayer)
int l_KillPlayer(lua_State* L);

// x, y, z = GetPlayerPosition(myPlayer)
int l_GetPlayerPosition(lua_State* L);

// SetPlayerPosition(myPlayer, x, y, z)
int l_SetPlayerPosition(lua_State* L);

// fuel = GetCarFuel(car)
int l_GetCarFuel(lua_State* L);

// SetCarFuel(car, fuel)
int l_SetCarFuel(lua_State* L);

// capacity = GetCarFuelCapacity(car)
int l_GetCarFuelCapacity(lua_State* L);

// isdriving = IsPlayerDriving(playerID)
int l_IsPlayerDriving(lua_State* L);

// seatno = GetPlayerSeatNumber(playerID)
int l_GetPlayerSeatNumber(lua_State* L);

// SpawnPlayerItem(myPlayer, "SKS", 3, true) -- last parameter determines if the item is spawned in inventory or on the floor
// might wanna consider a return type that is a user defined item type ( So you can check stuff later )
int l_SpawnPlayerItem(lua_State* L);

// itemStack = GetPlayerItemStackInHand(myPlayer)
int l_GetPlayerItemStackInHand(lua_State* L);

// itemName = GetItemStackName(itemStack)
int l_GetItemStackName(lua_State* L);

// RegisterCommandHandler("/onduty", onDutyHandler)
int l_RegisterCommandHandler(lua_State* L);

// UnregisterCommandHandler("/onduty", onDutyHandler)
int l_UnregisterCommandHandler(lua_State* L);

// ClearCommandHandlers("/onduty")
int l_ClearCommandHandlers(lua_State* L);

// RegisterEventHandler("onPlayerKilled", onKilledHandler)
// "onStartingEquipSetup", "onPlayerKilled", ...
int l_RegisterEventHandler(lua_State* L);

// CallLater(1000, callback, repeated?=false, param1, param2, param3 ..)
int l_CallLater(lua_State* L);

#include <string>
int CallCommandHandlers(std::string command, int playerID);

void ResetHandlers();