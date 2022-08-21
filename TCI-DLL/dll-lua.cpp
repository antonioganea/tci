#include "pch.h"
#include <iostream>



#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <fstream>

#include <thread>
#include <condition_variable>

#include "lua-api.h"


//int simBridge[1024];


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



/*


void ThreadA_Activity()
{
    std::cout << "Thread A started " << std::endl;

    cv.notify_one();//notify to ThreadB that he can start doing his job

    // wait for the Thread B
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return false; });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "end of Thread A" << std::endl;
}

*/


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


extern std::ofstream MyOutputFile;
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


char bootPath[512];

void initLua(const char* path) {
    MyOutputFile << "new state\n" << std::flush;
    state = luaL_newstate();
    MyOutputFile << "new state done\n" << std::flush;

    // Make standard libraries available in the Lua object
    //luaL_openlibs(state); <--- this crashes (maybe bcs of stdout stdin stderr - i/o lib )
    openCustomLuaLibs(state);

    //luaopen_math(state);
    MyOutputFile << "lua math done\n" << std::flush;

    //lua_pushcfunction(state, l_RegisterAsServerObject);
    //lua_setglobal(state, "RegisterAsServerObject");

    //lua_pushcfunction(state, l_SpawnVolga);
    //lua_setglobal(state, "SpawnOlga");

    lua_pushcfunction(state, l_AnnounceAll);
    lua_setglobal(state, "AnnounceAll");

    lua_pushcfunction(state, l_BroadcastMessage);
    lua_setglobal(state, "BroadcastMessage");

    lua_pushcfunction(state, l_ConsoleMessage);
    lua_setglobal(state, "ConsoleMessage");

    lua_pushcfunction(state, l_GetFloatsDemo);
    lua_setglobal(state, "GetFloatsDemo");

    strcpy_s(bootPath, path);

    MyOutputFile << "initLua done\n" << std::flush;
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
    OnSomething = 1592
};

bool justBooted = false; // unused

#include "bridge.h"

void LUA_INTERPRETER_UNEDITABLE() {

    MyOutputFile << "booting lua interpreter thread\n" << std::flush;

    while (isLuaPowered) {

        //DWORD* bridge = (DWORD*)DLL_BRIDGE;

        /*
        if (justBooted) {
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
                        return;
                    }
                }
            }

            ConsoleMessage("[tci] doFile executed!");
            MyOutputFile << "doFile done\n" << std::flush;

            justBooted = false;
        }
        */

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
                    return;
                }
            }
            }

            ConsoleMessage("[tci] doFile executed!");
            MyOutputFile << "doFile done\n" << std::flush;

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

        case (int)DayZServerCommands::OnSomething:
            //DLL_FLOATS_OUT[0];




            LuaexecuteLine("x, y, z = GetFloatsDemo(); ConsoleMessage(x .. ' ' .. y .. ' ' .. z)");
            break;

        default:
            MyOutputFile << "hit default in LUA THREAD. Something is WRONG\n" << std::flush;
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
    MyOutputFile << "setup\n" << std::flush;

    initLua(path);

    MyOutputFile << "setup2\n" << std::flush;

    inLua = true;

    DWORD* bridge = (DWORD*)DLL_BRIDGE;

    MyOutputFile << "setup3\n" << std::flush;
    //bridge[6] = (int)DayZServerCommands::JustBooted; // this should go through default / nothing
    *DLL_COMMAND = (int)DayZServerCommands::JustBooted;
    justBooted = true;
    MyOutputFile << "setup4\n" << std::flush;
    std::thread* ThreadA = new std::thread(LUA_INTERPRETER_UNEDITABLE);
            ThreadA->detach(); // Detaching actually helps
    MyOutputFile << "setup5\n" << std::flush;
    {
        //std::unique_lock<std::mutex> lk(m);
        MyOutputFile << "setup6\n" << std::flush;
        // ^^^ LAST THING TO BE PRINTED
        //cv.wait(lk, [] {return (inLua == false); });
        while(inLua==true){}
        MyOutputFile << "setup7\n" << std::flush;
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