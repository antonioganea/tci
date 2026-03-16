#include "magic-call.h"

#include <mutex>

#include "bridge.h"
#include "utils.h"

#include "dll-lua.h"

bool initializedLua = false;
bool SHOULD_LUA_HOTLOAD = false;

bool ESFirstCall = true; // signifies that OnGetControl is the first passthrough in an ES magic call cycle

#include <iostream>
#include <set>
#include "gui.h"

std::set<int> threadsVisisted;
void VisitedThreadsLogic();

volatile void OnGetControl()
{
    if (DLL_BRIDGE == NULL) {
        return;
    }

    if (*DLL_IS_MAGIC_CALL != 54) {
        return;
    }

    if (g_gameFiber == nullptr) {
        g_gameFiber = ConvertThreadToFiber(nullptr);
        if (g_gameFiber == nullptr && GetLastError() == ERROR_ALREADY_FIBER) {
            g_gameFiber = GetCurrentFiber();
        }
    }

    VisitedThreadsLogic();

    if (SHOULD_LUA_HOTLOAD && ESFirstCall && initializedLua) { // if lua needs to hotload and we're not in the middle of something..

        shutdownLuaFiber();

        initializedLua = false;
        SHOULD_LUA_HOTLOAD = false;
    }

    if (initializedLua)
    {
        goInLua();
    }
    else
    {
        if (*DLL_COMMAND == (int)DayZServerCommands::OnUpdatePass)
        {
            // This will ignore an update pass, but start a lua fiber
            startLuaFiber("lua\\script.lua");
            initializedLua = true;
        }
    }

    ESFirstCall = (*DLL_COMMAND) == 0;
}

void VisitedThreadsLogic()
{
    auto threadId = std::this_thread::get_id();
    int threadInt = threadId._Get_underlying_id();

    if (threadsVisisted.find(threadInt) == threadsVisisted.end())
    {
        //std::cout << "Visited thread " << threadInt << " for the first time" << std::endl;
        console.AddLog("# Visited thread %d for the first time\n", threadInt);
        threadsVisisted.emplace(threadInt);
    }
}