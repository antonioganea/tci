#include "magic-call.h"

#include <thread>
#include <mutex>

#include "bridge.h"
#include "utils.h"

extern std::thread* LUA_THREAD;
void freeLuaStateInternally();
std::thread* setup(const char* path);
void cleanup();
void goInLua();

void LuaexecuteLine(const char* s);

int ESCALL_COMMAND;
int watermark7 = 0;

bool preserved = false;

// Preserving is very important and must be done right, if not, the server will jam in the iterationCycle
// or potentially crash ( bcs of runtime errors )
void pushESCall() {
    //DWORD* bridge = (DWORD*)DLL_BRIDGE;
    ESCALL_COMMAND = *DLL_COMMAND;
    watermark7 = DLL_STRLEN_OUT[0]; // ex bridge[7]
    preserved = true;
}

void popESCall() {
    //DWORD* bridge = (DWORD*)DLL_BRIDGE;
    *DLL_COMMAND = ESCALL_COMMAND;
    DLL_STRLEN_OUT[0] = watermark7;
    preserved = false;
}

void debugCall() {
    //DWORD* bridge = (DWORD*)DLL_BRIDGE;
    if (DLL_BRIDGE != nullptr)
    {
        *DLL_TRIGGER_DEBUG_CALL = 12;
    }
}

bool initializedLua = false;
bool SHOULD_LUA_HOTLOAD = false;

bool ESFirstCall = true; // signifies that OnGetControl is the first passthrough in an ES magic call cycle




// HERE IS AN IDEA :
// Instead of wasting time on call preserving ....
// Allow hot reloads only on update events - that will get ignored anyway?

#include <iostream>
#include <set>
#include "gui.h"

std::set<int> threadsVisisted;

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

volatile void OnGetControl() {

    //MyOutputFile << "Something" << std::flush;

    if (DLL_BRIDGE == NULL) {
        return;
    }

    //DWORD* bridge = (DWORD*)DLL_BRIDGE;

    if (*DLL_IS_MAGIC_CALL != 54) {
        return;
    }

    VisitedThreadsLogic();

    // TODO(UpdatePass+LuaHotReload) : Need a more homogenous way of running setup()
    // not just on 2103 .... + there should be a global bool flag indicating that it should be hot-reloaded.
    // SHOULD_LUA_RELOAD == true .. something like this
    // Also, there needs to be a way to call setup() again.. but dispose the old state cleanly ( For hot reloads )

    //if (SHOULD_LUA_HOTLOAD && bridge[6] == 0 && initializedLua)
    if (SHOULD_LUA_HOTLOAD && ESFirstCall && initializedLua) { // if lua needs to hotload and we're not in the middle of something..
        freeLuaStateInternally();
        initializedLua = false;
        SHOULD_LUA_HOTLOAD = false;
    }

    if (!initializedLua) {

        // TODO: do we really need the bridge[6] temp swap ?
        // are we sure we can't just let it do whatever it wants to do in setup()?
        // ..... (should investigate for readability reasons )
        //int tempCode = bridge[6];
        // Might wanna :
        // 1) preserve all the call details from enfusion script, and execute them AFTER the lua setup...
        // or
        // 2) Dedicate a separate call from EnfusionScript that is specifically made for LuaSetup
        //      -> however, a system needs to ensure that ES calls that lua setup after we are injected(detouring), and only once
        //      -> when we want to HOT Reload we need to call ES to time a luasetupcall for later

        pushESCall();

        //bridge[6] = 0; // change this with non-magic code ( from enum DayzServerCommands )
        //pushESCallContext();   // to be implemented


        LUA_THREAD = setup("lua\\script.lua");

        // CURRENT STATUS :
        // If we call three broadcasts in a row, first one will be ignored because it is overwritten below ( bridge[6] = tempCode )
        // We need to delay the popESCallContext() until the Lua Thread places a 0 in bridge[6]

        initializedLua = true;

        //popESCallContext();   // to be implemented
        //bridge[6] = tempCode;
    }
    else {
        goInLua();

        if (initializedLua && ((*DLL_COMMAND) == 0)) {
            if (preserved) {
                popESCall();
                goInLua(); // WARNING! THIS goInLua is immediately after another one. Sometimes the cv.notify() is too fast and
                // the threads jam! Be advised. - CONFIRMED CAUSE OF PROBLEMS. needs to be fixed! - somehow it might work under some conditions
                // but it is KNOWN to cause jams.
            }
        }
    }

    ESFirstCall = (*DLL_COMMAND) == 0;
}