#include "pch.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <iostream>

#include "tci-api.h"

// Get rid of this when you clean the code. it was used for l_GetFloatsDemo
#include "bridge.h"

int l_GetFloatsDemo(lua_State* L) {

    
    lua_pushnumber(L, DLL_FLOATS_OUT[0]);
    lua_pushnumber(L, DLL_FLOATS_OUT[1]);
    lua_pushnumber(L, DLL_FLOATS_OUT[2]);

    return 3;
}

int l_ConsoleMessage(lua_State* L) {
    char buffer[256];
    strcpy(buffer, lua_tostring(L, 1));

    ConsoleMessage(buffer);

    return 0;
}

// BroadcastMessage("Our discord is discord.gg/something")
int l_BroadcastMessage(lua_State* L) {

    char buffer[256];
    strcpy(buffer, lua_tostring(L, 1));

    BroadcastMessage(buffer);

    return 0;
}

// SendPlayerMessage(myPlayer, "How do you do today?") -- Requires user defined player type
int l_SendPlayerMessage(lua_State* L) {

    int playerID = lua_tointeger(L, 1);

    char buffer[256];
    strcpy(buffer, lua_tostring(L, 2));

    SendPlayerMessage(playerID, buffer);

    return 0;
}

// car = SpawnCar("olga", x, y, z) -- Requires user defined car type
int l_SpawnCar(lua_State* L) {

    char buffer[256];
    strcpy(buffer, lua_tostring(L, 1));

    float x, y, z;

    x = lua_tonumber(L, 2);
    y = lua_tonumber(L, 3);
    z = lua_tonumber(L, 4);

    bool success = SpawnCar(buffer, x, y, z);

    lua_pushboolean(L, success);

    return 1;
}

// playerCount = GetPlayerCount()
int l_GetPlayerCount(lua_State* L) {
    lua_pushinteger(L, GetPlayerCount());
    return 1;
}

// player = GetPlayerBySteamID(76561198113938382) -- Requires user defined player type
int l_GetPlayerBySteamID(lua_State* L) {
    long long steamID = lua_tointeger(L, 1);
    lua_pushinteger(L, GetPlayerBySteamID(steamID));
    return 1;
}

// steamID = GetPlayerSteamID(myPlayer) -- Requires user defined player type
int l_GetPlayerSteamID(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    lua_pushinteger(L, GetPlayerSteamID(playerID));
    return 1;
}

// health = GetPlayerHealth(myPlayer) -- Requires user defined player type
int l_GetPlayerHealth(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    lua_pushnumber (L, GetPlayerHealth(playerID));
    return 1;
}

// maxHealth = GetPlayerMaxHealth(myPlayer) -- Requires user defined player type
int l_GetPlayerMaxHealth(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    lua_pushnumber(L, GetPlayerMaxHealth(playerID));
    return 1;
}

// SetPlayerHealth(myPlayer, health+10) -- Requires user defined player type
int l_SetPlayerHealth(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    float hp = lua_tonumber(L, 2);
    SetPlayerHealth(playerID, hp);
    return 0;
}

// blood = GetPlayerBlood(myPlayer) -- Requires user defined player type
int l_GetPlayerBlood(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    lua_pushnumber(L, GetPlayerBlood(playerID));
    return 1;
}

// maxBlood = GetPlayerMaxBlood(myPlayer) -- Requires user defined player type
int l_GetPlayerMaxBlood(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    lua_pushnumber(L, GetPlayerMaxBlood(playerID));
    return 1;
}

// SetPlayerBlood(myPlayer, blood+10) -- Requires user defined player type
int l_SetPlayerBlood(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    float hp = lua_tonumber(L, 2);
    SetPlayerBlood(playerID, hp);
    return 0;
}

// shock = GetPlayerShock(myPlayer) -- Requires user defined player type
int l_GetPlayerShock(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    lua_pushnumber(L, GetPlayerShock(playerID));
    return 1;
}

// maxShock = GetPlayerMaxShock(myPlayer) -- Requires user defined player type
int l_GetPlayerMaxShock(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    lua_pushnumber(L, GetPlayerMaxShock(playerID));
    return 1;
}

// SetPlayerShock(myPlayer, shock+10) -- Requires user defined player type
int l_SetPlayerShock(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    float hp = lua_tonumber(L, 2);
    SetPlayerShock(playerID, hp);
    return 0;
}

// KillPlayer(myPlayer) -- Requires user defined player type
int l_KillPlayer(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    KillPlayer(playerID);
    return 0;
}

// x, y, z = GetPlayerPosition(myPlayer) -- Requires user defined player type
int l_GetPlayerPosition(lua_State* L) {
    int playerID = lua_tointeger(L, 1);

    Vector3f pos = GetPlayerPosition(playerID);

    lua_pushnumber(L, pos.x);
    lua_pushnumber(L, pos.y);
    lua_pushnumber(L, pos.z);

    return 3;
}

// SetPlayerPosition(myPlayer, x, y, z)
int l_SetPlayerPosition(lua_State* L) {
    return 0;
}

// SpawnPlayerItem(myPlayer, "SKS", 3, true) -- last parameter determines if the item is spawned in inventory or on the floor
// might wanna consider a return type that is a user defined item type ( So you can check stuff later )
int l_SpawnPlayerItem(lua_State* L) {

    char buffer[256];
    strcpy(buffer, lua_tostring(L, 2));

    int playerID = lua_tointeger(L, 1);
    int quantity = lua_tointeger(L, 3);
    bool inHand = lua_toboolean(L, 4);

    SpawnPlayerItem(playerID, buffer, quantity, inHand);

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

#include <vector>
#include <map>
#include <string>

std::map<std::string, std::vector<int>> commandHandlers;

#include <fstream>

extern std::ofstream MyOutputFile;

// RegisterCommandHandler("/onduty", onDutyHandler)
int l_RegisterCommandHandler(lua_State* L) {

    MyOutputFile << "RegisterCommandHandler\n" << std::flush;

    // todo : check if the parameter is a function..

    if (lua_gettop(L) != 2) { // function takes two parameters
        return 0;
    }

    std::string commandName(lua_tostring(L, 1));

    MyOutputFile << commandName << " - command name\n" << std::flush;

    int r = luaL_ref(L, LUA_REGISTRYINDEX);

    MyOutputFile << "registered ref " << r << "\n" << std::flush;

    commandHandlers[commandName].push_back(r);

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

// CallLater(1000, callback, repeated?=false, param1, param2, param3 ..)
int l_CallLater(lua_State* L) {
    return 0;
}

extern lua_State* state;

int CallCommandHandlers(std::string command, int playerID) {

    MyOutputFile << "CallCommandHandlers\n" << std::flush;

    int calls = 0;

    std::vector<int>& handlers = commandHandlers[command];

    for (std::vector<int>::iterator it = handlers.begin(); it != handlers.end(); it++) {

        MyOutputFile << "calling ref " << *it << "\n" << std::flush;

        lua_rawgeti(state, LUA_REGISTRYINDEX, *it);
        lua_pushinteger(state, playerID);
        if (lua_pcall(state, 1, 0, 0) != 0){
            //error TODO - implement
        }
        calls++;
    }

    MyOutputFile << "returning " << calls << " calls\n" << std::flush;
    
    //luaL_unref(L, LUA_REGISTRYINDEX, r);

    return calls;
}

void ResetHandlers() {
    commandHandlers.clear();
}