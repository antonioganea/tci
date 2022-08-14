#include "pch.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <iostream>

#include "tci-api.h"

// RegisterAsServerObject( object )
int l_RegisterAsServerObject(lua_State* L) {
    std::cout << "Register as server object" << std::endl;
    int objectID;

    if (luaL_checkinteger(L, -1)) {
        objectID = lua_tonumber(L, -1);
    }

    std::cout << objectID << std::endl;

    return 0;
}

int l_SpawnVolga(lua_State* L) {
    SpawnOlga();
    /*
    std::cout << "[Lua] Spawning Olga ..." << std::endl;

    currentCommand = 54;

    goOutOfLua();

    currentCommand = 0;

    std::cout << "[Lua] Spawned Olga" << std::endl;
    */

    return 0;
}

int l_AnnounceAll(lua_State* L) {
    AnnounceAll();

    return 0;
}

// BroadcastMessage("Our discord is discord.gg/something")
int l_BroadcastMessage(lua_State* L) {
    return 0;
}

// SendPlayerMessage(myPlayer, "How do you do today?") -- Requires user defined player type
int l_SendPlayerMessage(lua_State* L) {
    return 0;
}

// car = SpawnCar("olga", x, y, z) -- Requires user defined car type
int l_SpawnCar(lua_State* L) {
    return 0;
}

// playerCount = GetPlayerCount()
int l_GetPlayerCount(lua_State* L) {
    return 0;
}

// player = GetPlayerBySteamID(76561198113938382) -- Requires user defined player type
int l_GetPlayerBySteamID(lua_State* L) {
    return 0;
}

// steamID = GetPlayerSteamID(myPlayer) -- Requires user defined player type
int l_GetPlayerSteamID(lua_State* L) {
    return 0;
}

// health = GetPlayerHealth(myPlayer) -- Requires user defined player type
int l_GetPlayerHealth(lua_State* L) {
    return 0;
}

// SetPlayerHealth(myPlayer, health+10) -- Requires user defined player type
int l_SetPlayerHealth(lua_State* L) {
    return 0;
}

// blood = GetPlayerBlood(myPlayer) -- Requires user defined player type
int l_GetPlayerBlood(lua_State* L) {
    return 0;
}

// SetPlayerBlood(myPlayer, blood+10) -- Requires user defined player type
int l_SetPlayerBlood(lua_State* L) {
    return 0;
}

// shock = GetPlayerShock(myPlayer) -- Requires user defined player type
int l_GetPlayerShock(lua_State* L) {
    return 0;
}

// SetPlayerShock(myPlayer, shock+10) -- Requires user defined player type
int l_SetPlayerShock(lua_State* L) {
    return 0;
}

// KillPlayer(myPlayer) -- Requires user defined player type
int l_KillPlayer(lua_State* L) {
    return 0;
}

// x, y, z = GetPlayerPosition(myPlayer) -- Requires user defined player type
int l_GetPlayerPosition(lua_State* L) {
    return 0;
}

// SetPlayerPosition(myPlayer, x, y, z)
int l_SetPlayerPosition(lua_State* L) {
    return 0;
}

// SpawnPlayerItem(myPlayer, "SKS", 3, true) -- last parameter determines if the item is spawned in inventory or on the floor
// might wanna consider a return type that is a user defined item type ( So you can check stuff later )
int l_SpawnPlayerItem(lua_State* L) {
    return 0;
}

// itemStack = GetPlayerItemStackInHand(myPlayer) -- Requires user defined item type
int l_GetPlayerItemStackInHand(lua_State* L) {
    return 0;
}

// itemName = GetItemStackName(itemStack) -- Requires user defined item type
int l_GetItemStackName(lua_State* L) {
    return 0;
}

// RegisterCommandHandler("/onduty", onDutyHandler)
int l_RegisterCommandHandler(lua_State* L) {
    return 0;
}

// UnregisterCommandHandler("/onduty", onDutyHandler)
int l_UnregisterCommandHandler(lua_State* L) {
    return 0;
}

// ClearCommandHandlers("/onduty")
int l_ClearCommandHandlers(lua_State* L) {
    return 0;
}

// RegisterEventHandler("onPlayerKilled", onKilledHandler)
// "onStartingEquipSetup", "onPlayerKilled", ...
int l_RegisterEventHandler(lua_State* L) {
    return 0;
}