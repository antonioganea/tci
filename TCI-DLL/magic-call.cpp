#include "magic-call.h"

#include <thread>
#include <mutex>

/*
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
*/

#include "bridge.h"
#include "utils.h"

extern std::thread* LUA_THREAD;
void freeLuaStateInternally();
std::thread* setup(const char* path);
void cleanup();
void goInLua();

void LuaexecuteLine(const char* s);

#ifdef DESKTOP_DEBUG_FILE
std::ofstream MyOutputFile("C:\\Users\\Antonio\\Desktop\\DLL-Log-debugging.txt");
#endif

bool MockThreadSupport = false;
std::mutex mockMutex;
std::condition_variable mockcv;

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
    *DLL_TRIGGER_DEBUG_CALL = 12;
}

bool initializedLua = false;
bool SHOULD_LUA_HOTLOAD = false;

bool ESFirstCall = true; // signifies that OnGetControl is the first passthrough in an ES magic call cycle




// HERE IS AN IDEA :
// Instead of wasting time on call preserving ....
// Allow hot reloads only on update events - that will get ignored anyway?


volatile void OnGetControl() {

    //MyOutputFile << "Something" << std::flush;

    if (DLL_BRIDGE == NULL) {
        return;
    }

    //DWORD* bridge = (DWORD*)DLL_BRIDGE;

    if (*DLL_IS_MAGIC_CALL != 54) {
        return;
    }

    // TODO(UpdatePass+LuaHotReload) : Need a more homogenous way of running setup()
    // not just on 2103 .... + there should be a global bool flag indicating that it should be hot-reloaded.
    // SHOULD_LUA_RELOAD == true .. something like this
    // Also, there needs to be a way to call setup() again.. but dispose the old state cleanly ( For hot reloads )

#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "FX0\n" << std::flush;
#endif

    //if (SHOULD_LUA_HOTLOAD && bridge[6] == 0 && initializedLua)
    if (SHOULD_LUA_HOTLOAD && ESFirstCall && initializedLua) { // if lua needs to hotload and we're not in the middle of something..
        freeLuaStateInternally();
        initializedLua = false;
        SHOULD_LUA_HOTLOAD = false;
#ifdef DESKTOP_DEBUG_FILE
        MyOutputFile << "FX-HR\n" << std::flush;
#endif
    }

    if (!initializedLua) {
#ifdef DESKTOP_DEBUG_FILE
        MyOutputFile << "FX-notInitialized\n" << std::flush;
#endif

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


        LUA_THREAD = setup("C:\\Program Files (x86)\\Steam\\steamapps\\common\\DayZServer\\lua\\script.lua");

        // CURRENT STATUS :
        // If we call three broadcasts in a row, first one will be ignored because it is overwritten below ( bridge[6] = tempCode )
        // We need to delay the popESCallContext() until the Lua Thread places a 0 in bridge[6]

        initializedLua = true;

        //popESCallContext();   // to be implemented
        //bridge[6] = tempCode;



    }
    else {
#ifdef DESKTOP_DEBUG_FILE
        MyOutputFile << "FX-else\n" << std::flush;
#endif
        goInLua();
        // should try one more time to remove spinlock from main thread????? - tried, doesn't work
        // also test with OTHER mutex and cv. - tried, didn't work

        if (initializedLua && ((*DLL_COMMAND) == 0)) {
#ifdef DESKTOP_DEBUG_FILE
            MyOutputFile << "FX-ending\n" << std::flush;
#endif
            if (preserved) {
#ifdef DESKTOP_DEBUG_FILE
                MyOutputFile << "FX-popES\n" << std::flush;
#endif
                popESCall();
                goInLua(); // WARNING! THIS goInLua is immediately after another one. Sometimes the cv.notify() is too fast and
                // the threads jam! Be advised. - CONFIRMED CAUSE OF PROBLEMS. needs to be fixed! - somehow it might work under some conditions
                // but it is KNOWN to cause jams.
            }
        }
    }

    ESFirstCall = (*DLL_COMMAND) == 0;


    //MyOutputFile << "DDD\n" << std::flush;



    //MyOutputFile << "FFF\n" << std::flush;
#ifdef DESKTOP_DEBUG_FILE
    MyOutputFile << "FX-finale\n" << std::flush;
#endif


    /*
    if (bridge[6] == 2103) {
        MockThreadSupport = false;
        MyOutputFile << "pre-thread\n" << std::flush;
        std::thread newFunnyThread(simpleThread);
        //newFunnyThread.join(); // If we don't join the thread, it crashes the game
        newFunnyThread.detach(); // Detaching or joining works.

        MyOutputFile << "post-thread - wait stage\n" << std::flush;

        //std::unique_lock<std::mutex> lk(mockMutex);
        //MyOutputFile << "setup6\n" << std::flush;
        // ^^^ LAST THING TO BE PRINTED
        //mockcv.wait(lk, [] {return (MockThreadSupport == true); });

        //mockcv.wait(lk); // <--- the wait crashes the execution

        while(MockThreadSupport==false){//busy waiting - spin lock
        }

        MyOutputFile << "post-thread - post wait\n" << std::flush;
    }
    */



    // CURRENT PROBLEM *LATEST* : chekc if CV's work in threads .. and so on ..
    /*
    if (LUA_THREAD == NULL) {
        MyOutputFile << "Lua thread first time setup from server control\n" << std::flush;
        LUA_THREAD = setup(); // <--- as this doesn't work, THE LAUNCHING OF A THREAD SHOULD BE TESTED.
        //LUA_THREAD->detach();
        MyOutputFile << "Lua thread first time setup done\n" << std::flush;
    }
    */


    /*
    if (bridge[6] == 2103) {
        // CURRENT STATUS PROBLEM
        // This breaks execution :
        //LuaexecuteLine("function smecherie() local a = 10; a = a + 10; return a; end local b = smecherie()");
        //It might be because the Lua has to be invoked from another DLL - possible solution - try to compile as included lib...
        // Another TEST solution - inject the Lua.DLL BEFORE injecting the TCI dll // Tested - Not working


        MyOutputFile << "/lua executed\n" << std::flush;

        lua_State* newLuaState = luaL_newstate(); // <- this is where it crashes. apparently it's all about running code from other DLL
        MyOutputFile << "made new state\n" << std::flush;

        //luaL_openlibs(newLuaState); // <-- this crashed, probably because of the I/O lib trying to open stdout stdin stderr
        //MyOutputFile << "opened libs\n" << std::flush;

        luaopen_math(newLuaState); // <-- this worked
        MyOutputFile << "math lib\n" << std::flush;

        lua_pushcfunction(newLuaState, l_AnnounceAll2);
        lua_setglobal(newLuaState, "annoncer");

        MyOutputFile << "annoncer - registered\n" << std::flush;

        bridge[6] = 0;

        int result = luaL_loadstring(newLuaState, "a = 10; annoncer();");
        MyOutputFile << "loadstring worked\n" << std::flush;

        switch (result) {
        case LUA_ERRSYNTAX:
            //puts("[Lua] Error executing line ( syntax ) !");
            break;
        case LUA_ERRMEM:
            //puts("[Lua] Error executing line ( memory ) !");
            break;
        default: {
            int res = lua_pcall(newLuaState, 0, LUA_MULTRET, 0);
            if (res != LUA_OK) {
                //print_error(state);
                return;
            }
        }
        }

        //https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking?redirectedfrom=MSDN
        //bridge[6] = 1002;
    }
    else {
        bridge[6] = 0;
    }
    */

    /*
    MyOutputFile << "before goInLua\n" << std::flush;
    // Current status problem : execution jams when calling /lua
    goInLua();
    MyOutputFile << "after goInLua\n" << std::flush;
    */

    /*
    if (bridge[6] == 2103) {
        bridge[6] = 1002;
    }
    else {
        bridge[6] = 0;
    }
    */


    //goInLua();



}