
#include <iostream>

#include <Windows.h>

extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

#include <fstream>

#include "bridge.h"
#include "lua-api.h"
#include "tci-api.h"
#include "dll-lua.h"
#include "lua-state.h"

void LUA_INTERPRETER_FIBER()
{
    while (true) {
        switch (*DLL_COMMAND)
        {
            case (int)DayZServerCommands::OnUpdate:

                break;

            case (int)DayZServerCommands::OnKilled:

                break;
            case (int)DayZServerCommands::JustBooted:
            {
                ConsoleMessage("[tci] executing doFile...");
                JustBooted();
                ConsoleMessage("[tci] doFile executed!");

                break;
            }

            case (int)DayZServerCommands::OnCommand:
                //LuaexecuteLine("SpawnOlga();a = 5;SpawnOlga();b = 5");
                LuaexecuteLine("BroadcastMessage(os.date('%X', os.time())); ConsoleMessage('someone executed /lua')");

                break;

            case (int)DayZServerCommands::OnUpdatePass:
                CallUpdateHandler();
                break;

            case (int)DayZServerCommands::OnGenericCommand:
                if (CallCommandHandlers(*DLL_IN_STR1, DLL_INTS_OUT[0]) == 0)
                {
                    ConsoleMessage("Unknown command in interpreter.");
                }
                break;

            default:
                break;

        }

        *DLL_COMMAND = 0;
        goOutOfLua();
    }
}

void startLuaFiber(const char* path)
{
    if (g_luaFiber != NULL) {
        return;
    }

    createLuaState(path);

    *DLL_COMMAND = (int)DayZServerCommands::JustBooted;

    g_luaFiber = CreateFiber(0, [](LPVOID param) -> void {
        LUA_INTERPRETER_FIBER();
        SwitchToFiber(g_gameFiber);
    }, nullptr);

    goInLua();
}

std::atomic<bool> g_luaShouldExit = false;

void shutdownLuaFiber()
{
    g_luaShouldExit = true;
    SwitchToFiber(g_luaFiber);   // let it clean up and return
    // now fiber has exited cleanly
    DeleteFiber(g_luaFiber);      // safe — stack is clean
    g_luaFiber = nullptr;
}

LPVOID g_gameFiber = nullptr, g_luaFiber = nullptr;

void goOutOfLua()
{
    SwitchToFiber(g_gameFiber);

    // When we come back, check if we should abort
    if (g_luaShouldExit) {
        g_luaShouldExit = false;
        // Clean up anything on this fiber's stack/heap here
        freeLuaStateInternally();
        SwitchToFiber(g_gameFiber); // return to game, never come back
        // unreachable
    }
}

void goInLua()
{
    SwitchToFiber(g_luaFiber);
}