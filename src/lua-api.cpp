#include <TCICore/lua-api.h>
#include <TCICore/tci-api.h>
#include <TCICore/extensions-api.h>
#include "lua/lua.h"

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

    int car = SpawnCar(buffer, x, y, z);

    lua_pushinteger(L, car);

    return 1;
}

// car = GetPlayerCar(playerID)
int l_GetPlayerCar(lua_State* L) {
    int playerID = lua_tointeger(L, 1);
    lua_pushinteger(L, GetPlayerCar(playerID));
    return 1;
}

// playerCount = GetPlayerCount()
int l_GetPlayerCount(lua_State* L) {
    lua_pushinteger(L, GetPlayerCount());
    return 1;
}

// player = GetPlayerBySteamID(steamID) -- Requires user defined player type
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

// fuel = GetCarFuel(car)
int l_GetCarFuel(lua_State* L) {
    int car = lua_tointeger(L, 1);
    lua_pushnumber(L, GetCarFuel(car));
    return 1;
}

// SetCarFuel(car, fuel)
int l_SetCarFuel(lua_State* L) {
    int car = lua_tointeger(L, 1);
    float fuel = lua_tonumber(L, 2);
    SetCarFuel(car, fuel);
    return 0;
}

// capacity = GetCarFuelCapacity(car)
int l_GetCarFuelCapacity(lua_State* L) {
    int car = lua_tointeger(L, 1);
    lua_pushnumber(L, GetCarFuelCapacity(car));
    return 1;
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

// RegisterCommandHandler("/onduty", onDutyHandler)
int l_RegisterCommandHandler(lua_State* L) {

    // todo : check if the parameter is a function..

    if (lua_gettop(L) != 2) { // function takes two parameters
        return 0;
    }

    std::string commandName(lua_tostring(L, 1));

    int r = luaL_ref(L, LUA_REGISTRYINDEX);

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

int updateHandler = 0;

// RegisterEventHandler("onPlayerKilled", onKilledHandler)
// "onStartingEquipSetup", "onPlayerKilled", ...
int l_RegisterEventHandler(lua_State* L) {
    // todo : check if the parameter is a function..

    if (lua_gettop(L) != 2) { // function takes two parameters
        return 0;
    }

    std::string eventName(lua_tostring(L, 1));

    int callback = luaL_ref(L, LUA_REGISTRYINDEX);

    if (eventName == "update")
    {
        if (callback != 0)
        {
            updateHandler = callback;
        }
    }

    return 0;
}

// CallLater(1000, callback, repeated?=false, param1, param2, param3 ..)
int l_CallLater(lua_State* L) {
    return 0;
}

// LoadExtension(extName)
int l_LoadExtension(lua_State *L) {
    if (lua_gettop(L) != 1) return 0;

    const char *arg1 = lua_tostring(L, 1);

    LoadExtension(arg1);
    return 0;
}

// FreeExtension(extName)
int l_FreeExtension(lua_State *L) {
    if (lua_gettop(L) != 1) return 0;

    const char *arg1 = lua_tostring(L, 1);

    FreeExtension(arg1);
    return 0;
}

// CallExtension(extName, funcName, param1, param2, param3 ..)
int l_CallExtension(lua_State *L) {
    int argc = lua_gettop(L);

    if (argc < 2) return 0;

    const char *arg1 = lua_tostring(L, 1);
    const char *arg2 = lua_tostring(L, 2);

    if (argc == 2) {
        CallExtension(arg1, arg2, 0, nullptr);
        return 0;
    }

    int extArgc = argc - 2;
    const char *argv_stack[64 + 1];
    if (argc > 64) return 0;

    for (int i = 0; i < argc; i++) {
        int lidx = i + 3;

        argv_stack[i] = lua_tostring(L, lidx);
    }
    argv_stack[argc] = nullptr;

    CallExtension(arg1, arg2, extArgc, argv_stack);
    return 0;
}

extern lua_State* state;

#include <regex>

std::vector<std::string> ParseCommandArgs(std::string command) {
    std::vector<std::string> args;

    std::regex arg_regex(R"([^\s"]+|"[^"]*")");

    auto wb = std::sregex_iterator(command.begin(), command.end(), arg_regex);
    auto we = std::sregex_iterator();

    for (std::sregex_iterator i = wb; i != we; i++) {
        std::string match = i->str();

        if (match.size() >= 2 && match.front() == '"' && match.back() == '"') {
            match = match.substr(1, match.size() - 2);
        }

        args.push_back(match);
    }

    return args;
}

int CallCommandHandlers(std::string command, int playerID) {
    int calls = 0;

    std::vector<std::string> args = ParseCommandArgs(command);
    if (args.empty()) return 0;

    std::vector<int>& handlers = commandHandlers[args[0]];

    for (std::vector<int>::iterator it = handlers.begin(); it != handlers.end(); it++) {
        lua_rawgeti(state, LUA_REGISTRYINDEX, *it);
        lua_pushinteger(state, playerID);
        int argsCount = 1;

        for (size_t i = 1; i < args.size(); i++) {
            lua_pushstring(state, args[i].c_str());
            argsCount++;
        }

        if (lua_pcall(state, argsCount, 0, 0) != 0) {
            const char *errMsg = lua_tostring(state, -1);
            if (errMsg) {
                ConsoleMessage((std::string("Lua Error: ") + errMsg).c_str());
            }

            lua_pop(state, 1);
        }
        calls++;
    }
    
    //luaL_unref(L, LUA_REGISTRYINDEX, r);

    return calls;
}

void CallUpdateHandler()
{
    if (updateHandler != 0)
    {
        lua_rawgeti(state, LUA_REGISTRYINDEX, updateHandler);
        if (lua_pcall(state, 0, 0, 0) != 0){
            const char *errMsg = lua_tostring(state, -1);
            if (errMsg) {
                ConsoleMessage((std::string("Lua Error: ") + errMsg).c_str());
            }

            lua_pop(state, 1);
        }
    }
}

void ResetHandlers() {
    commandHandlers.clear();
    updateHandler = 0;
}