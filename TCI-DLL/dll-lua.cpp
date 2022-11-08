
#include <iostream>

#include <Windows.h>

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

#include <fstream>

#include <thread>
#include <condition_variable>

#include "lua-api.h"

std::mutex m;
std::condition_variable cv;

bool inLua = false;

void goInLua() {
    inLua = true;
    cv.notify_one();
    {
        //std::unique_lock<std::mutex> lk(m);
        //cv.wait(lk, [] {return (inLua == false); });
        while (inLua == true) {/* spinlock */ }
    }
}

bool isLuaPowered = true;

void goOutOfLua() {
    inLua = false;
    //cv.notify_one();
    if (isLuaPowered) // This here is meant to be a condition for the thread to skip Lua demanding data ( to end the LUA THREAD fast on final cleanup )
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return (inLua == true); });
        //while (inLua == false) {/* spinlock */}
    }
}

#include "tci-api.h"

void print_error_console(lua_State* state) {
    // The error message is on top of the stack.
    // Fetch it, print it and then pop it off the stack.
    const char* message = lua_tostring(state, -1);
    //puts(message);
    ConsoleMessage(message);
    lua_pop(state, 1);
}


lua_State* state;


#ifdef DESKTOP_DEBUG_FILE
extern std::ofstream MyOutputFile;
#endif
extern BYTE* DLL_BRIDGE;



static const luaL_Reg libsToLoad[] = {
  {LUA_GNAME, luaopen_base}, // works
  //{LUA_LOADLIBNAME, luaopen_package}, // not working - confirmed
  {LUA_COLIBNAME, luaopen_coroutine}, // works
  {LUA_TABLIBNAME, luaopen_table}, // works
  {LUA_IOLIBNAME, luaopen_io}, // works
  {LUA_OSLIBNAME, luaopen_os}, // works
  {LUA_STRLIBNAME, luaopen_string}, // works
  {LUA_MATHLIBNAME, luaopen_math}, // works
  {LUA_UTF8LIBNAME, luaopen_utf8}, // works
  {LUA_DBLIBNAME, luaopen_debug}, // works
  {NULL, NULL}
};


void openCustomLuaLibs(lua_State* L) {
    const luaL_Reg* lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = libsToLoad; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib */
    }
}

static const luaL_Reg luaApiFunctions[] = {
  {"ConsoleMessage", l_ConsoleMessage},
  {"BroadcastMessage", l_BroadcastMessage},
  {"SendPlayerMessage", l_SendPlayerMessage},

  {"SpawnCar", l_SpawnCar},
  {"GetPlayerCar", l_GetPlayerCar},

  {"GetPlayerCount", l_GetPlayerCount},

  {"GetPlayerBySteamID", l_GetPlayerBySteamID},
  {"GetPlayerSteamID", l_GetPlayerSteamID},

  {"GetPlayerHealth", l_GetPlayerHealth},
  {"GetPlayerMaxHealth", l_GetPlayerMaxHealth},
  {"SetPlayerHealth", l_SetPlayerHealth},

  {"GetPlayerBlood", l_GetPlayerBlood},
  {"GetPlayerMaxBlood", l_GetPlayerMaxBlood},
  {"SetPlayerBlood", l_SetPlayerBlood},

  {"GetPlayerShock", l_GetPlayerShock},
  {"GetPlayerMaxShock", l_GetPlayerMaxShock},
  {"SetPlayerShock", l_SetPlayerShock},

  {"KillPlayer", l_KillPlayer},

  {"RegisterCommandHandler", l_RegisterCommandHandler},

  {"GetPlayerPosition", l_GetPlayerPosition},
  {"SetPlayerPosition", l_SetPlayerPosition},

  {"GetCarFuel", l_GetCarFuel},
  {"SetCarFuel", l_SetCarFuel},
  {"GetCarFuelCapacity", l_GetCarFuelCapacity},

  {"SpawnPlayerItem", l_SpawnPlayerItem},

  {NULL, NULL}
};

void registerLuaApiFunctions(lua_State* L) {
    const luaL_Reg* func;
    for (func = luaApiFunctions; func->func; func++) {
        lua_pushcfunction(state, func->func);
        lua_setglobal(state, func->name);
    }
}

char bootPath[512];

void initLua(const char* path) {
#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "new state\n" << std::flush;
#endif
    state = luaL_newstate();
#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "new state done\n" << std::flush;
#endif

    // Make standard libraries available in the Lua object
    // luaL_openlibs(state); <--- this crashes ( because of luaopen_package )
    openCustomLuaLibs(state);

    registerLuaApiFunctions(state);

    strcpy_s(bootPath, path);

#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "initLua done\n" << std::flush;
#endif
}

void print_error(lua_State* state) {
    // The error message is on top of the stack.
    // Fetch it, print it and then pop it off the stack.
    const char* message = lua_tostring(state, -1);
    puts(message);
    lua_pop(state, 1);
}

void LuaexecuteLine(const char* s) {
    int result = luaL_loadstring(state, s);
    switch (result) {
        case LUA_ERRSYNTAX:
            puts("[Lua] Error executing line ( syntax ) !");
            break;
        case LUA_ERRMEM:
            puts("[Lua] Error executing line ( memory ) !");
            break;
        default: {
            int res = lua_pcall(state, 0, LUA_MULTRET, 0);
            if (res != LUA_OK) {
                print_error(state);
                return;
            }
        }
    }
}

enum class DayZServerCommands {
    Nothing,
    OnUpdate,
    OnKilled,
    JustBooted,
    OnCommand = 2103,
    OnUpdatePass = 4912,
    OnGenericCommand = 5522
};

#include "bridge.h"

void LUA_INTERPRETER_UNEDITABLE() {

#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "booting lua interpreter thread\n" << std::flush;
#endif

    while (isLuaPowered) {
        switch (*DLL_COMMAND)
        {
            case (int)DayZServerCommands::OnUpdate:

                break;

            case (int)DayZServerCommands::OnKilled:

                break;
            case (int)DayZServerCommands::JustBooted: {

                ConsoleMessage("[tci] executing doFile...");

                int res = luaL_dofile(state, bootPath);
                switch (res) {
                case LUA_ERRSYNTAX:
                    //puts("[Lua] Error executing line ( syntax ) !");
                    break;
                case LUA_ERRMEM:
                    //puts("[Lua] Error executing line ( memory ) !");
                    break;
                default: {
                    //int res = lua_pcall(state, 0, LUA_MULTRET, 0);
                    if (res != LUA_OK) {
                        print_error_console(state);
                        break;
                    }
                }
                }

                ConsoleMessage("[tci] doFile executed!");
    #ifdef DESKTOP_DEBUG_FILE
                MyOutputFile << "doFile done\n" << std::flush;
    #endif

                break;
            }

            case (int)DayZServerCommands::OnCommand:
                //LuaexecuteLine("SpawnOlga();a = 5;SpawnOlga();b = 5");
                LuaexecuteLine("BroadcastMessage(os.date('%X', os.time())); ConsoleMessage('someone executed /lua')");

                break;

            case (int)DayZServerCommands::OnUpdatePass:
                LuaexecuteLine("ConsoleMessage('OnUpdatePass')");
                // ...

                break;

            case (int)DayZServerCommands::OnGenericCommand:
    #ifdef DESKTOP_DEBUG_FILE
                MyOutputFile << "OnGenericCommand\n" << std::flush;
                MyOutputFile << std::hex << (long long)DLL_IN_STR1 << std::dec << '\n' << std::flush;
                MyOutputFile << std::hex << (long long)(*DLL_IN_STR1) << std::dec << '\n' << std::flush;
    #endif
                if (CallCommandHandlers(*DLL_IN_STR1, DLL_INTS_OUT[0]) > 0) {
    #ifdef DESKTOP_DEBUG_FILE
                    MyOutputFile << "fa1\n" << std::flush;
    #endif
                }
                else {
    #ifdef DESKTOP_DEBUG_FILE
                    MyOutputFile << "fa2\n" << std::flush;
    #endif
                    ConsoleMessage("Unknown command in interpreter.");
                }
                break;

            default:
    #ifdef DESKTOP_DEBUG_FILE
                MyOutputFile << "hit default in LUA THREAD. Something is WRONG\n" << std::flush;
    #endif
                break;

        }

        //MyOutputFile << "exiting\n" << std::flush;
        *DLL_COMMAND = 0;
        goOutOfLua();
    }

    //inLua = false;
    //cv.notify_one();
}

std::thread* LUA_THREAD = NULL;


void freeLuaStateInternally() {
    lua_close(state);
    ResetHandlers();
}

// This is called when you already have a thread running
void createLuaState(const char* path) { // TODO : name it more appropiately
    initLua(path);
    //DWORD* bridge = (DWORD*)DLL_BRIDGE;
    //bridge[6] = (int)DayZServerCommands::JustBooted;
    *DLL_COMMAND = (int)DayZServerCommands::JustBooted;
    goInLua();
}

std::thread* setup(const char* path) {

    if (LUA_THREAD != NULL) {
        createLuaState(path);
        return LUA_THREAD;
    }


    //for (int i = 0; i < 500; i++) { DLL_BRIDGE[i] = 0; }
#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "setup\n" << std::flush;
#endif

    initLua(path);

#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "setup2\n" << std::flush;
#endif

    inLua = true;

    DWORD* bridge = (DWORD*)DLL_BRIDGE;

#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "setup3\n" << std::flush;
#endif
    //bridge[6] = (int)DayZServerCommands::JustBooted; // this should go through default / nothing
    *DLL_COMMAND = (int)DayZServerCommands::JustBooted;

#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "setup4\n" << std::flush;
#endif

    std::thread* ThreadA = new std::thread(LUA_INTERPRETER_UNEDITABLE);
            ThreadA->detach(); // Detaching actually helps

#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "setup5\n" << std::flush;
#endif
    {
        //std::unique_lock<std::mutex> lk(m);
#ifdef DESKTOP_DEBUG_FILE
        MyOutputFile << "setup6\n" << std::flush;
#endif
        // ^^^ LAST THING TO BE PRINTED
        //cv.wait(lk, [] {return (inLua == false); });
        while(inLua==true){}
#ifdef DESKTOP_DEBUG_FILE
        MyOutputFile << "setup7\n" << std::flush;
#endif
    }

    return ThreadA;
}

void cleanup() {
    // ending sequence
    isLuaPowered = false;
    inLua = true;
    cv.notify_one();
    // with the disallocation
    LUA_THREAD->join();
    delete LUA_THREAD;
}

/*
int main2()
{
    LUA_THREAD = setup("C:\\Program Files (x86)\\Steam\\steamapps\\common\\DayZServer\\lua\\script.lua");

    //std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "[[[[[Sloop]]]]]" << std::endl;
    //std::this_thread::sleep_for(std::chrono::seconds(1));


    DLL_BRIDGE[0] = (int)DayZServerCommands::OnCommand;
    goInLua();

    std::cout << "SKIB" << std::endl;
    goInLua();

    std::cout << "Frib" << std::endl;

    // TODO : if we delete these two lines, it will stuck the lua thread...
    // (make sure the calls are in a while(true) {notify()} loop)
    // OUTDATED / STALE / OLD NOTE
    //goInLua();
    //std::cout << "Frib2" << std::endl;

    // not a great solution:
    //LUA_THREAD->detach();
    //delete LUA_THREAD;

    cleanup();

    return 0;
}
*/