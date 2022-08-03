#include "pch.h"
#include <iostream>



#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <fstream>

#include <thread>
#include <condition_variable>


//int simBridge[1024];

extern BYTE* DLL_BRIDGE;


std::mutex m;
std::condition_variable cv;

bool inLua = false;

int currentCommand = 0;

void goInLua() {
    inLua = true;
    cv.notify_one();
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return (inLua == false); });
    }
}


bool isLuaPowered = true;

void goOutOfLua() {
    inLua = false;
    cv.notify_one();
    if (isLuaPowered) // This here is meant to be a condition for the thread to skip Lua demanding data ( to end the LUA THREAD fast on final cleanup )
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return (inLua == true); });
    }
}

extern std::ofstream MyOutputFile;

void AnnounceAll() {
    DWORD* bridge = (DWORD*)DLL_BRIDGE;
    bridge[6] = 1002;
    MyOutputFile << "AnnounceAll -internally\n" << std::flush;
    goOutOfLua();
}

//Call from Lua to Cpp
void SpawnOlga() {
    std::cout << "Spawn Olga point" << std::endl;

    goOutOfLua();

    std::cout << "Spawn Olga done" << std::endl;

    // Writes to shared_mem (bridge)
    // NEEDS TO PASS CONTROL TO DayZServer_x64
    //giveControl();
}

void SetDay() {
    std::cout << "Set day point" << std::endl;

    goOutOfLua();

    /*
    cv.notify_one();
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return false; });
    }
    */

    std::cout << "Set day done" << std::endl;

    // Writes to shared_mem (bridge)
    // NEEDS TO PASS CONTROL TO DayZServer_x64
    //giveControl();
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

lua_State* state;



void initLua() {
    MyOutputFile << "new state\n" << std::flush;
    state = luaL_newstate();
    MyOutputFile << "new state done\n" << std::flush;

    // Make standard libraries available in the Lua object
    //luaL_openlibs(state); <--- this crashes (maybe bcs of stdout stdin stderr - i/o lib )
    luaopen_math(state);
    MyOutputFile << "lua math done\n" << std::flush;

    //lua_pushcfunction(state, l_RegisterAsServerObject);
    //lua_setglobal(state, "RegisterAsServerObject");

    //lua_pushcfunction(state, l_SpawnVolga);
    //lua_setglobal(state, "SpawnOlga");

    lua_pushcfunction(state, l_AnnounceAll);
    lua_setglobal(state, "AnnounceAll");

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
    OnCommand = 2103
};

void LUA_INTERPRETER_UNEDITABLE() {

    MyOutputFile << "booting lua interpreter thread\n" << std::flush;

    while (isLuaPowered) {

        DWORD* bridge = (DWORD*)DLL_BRIDGE;

        switch (bridge[6])
        {

        case (int)DayZServerCommands::OnUpdate:

            break;

        case (int)DayZServerCommands::OnKilled:

            break;

        case (int)DayZServerCommands::OnCommand:
            //LuaexecuteLine("SpawnOlga();a = 5;SpawnOlga();b = 5");
            LuaexecuteLine("AnnounceAll()");

            break;


        default:
            MyOutputFile << "hit default\n" << std::flush;
            break;

        }

        MyOutputFile << "exiting\n" << std::flush;
        bridge[6] = 0;
        goOutOfLua();
    }

    //inLua = false;
    //cv.notify_one();
}

std::thread* LUA_THREAD = NULL;




std::thread* setup() {
    //for (int i = 0; i < 500; i++) { DLL_BRIDGE[i] = 0; }
    MyOutputFile << "setup\n" << std::flush;

    initLua();

    MyOutputFile << "setup2\n" << std::flush;

    inLua = true;

    DWORD* bridge = (DWORD*)DLL_BRIDGE;

    MyOutputFile << "setup3\n" << std::flush;
    bridge[6] = (int)DayZServerCommands::Nothing; // this should go through default / nothing
    MyOutputFile << "setup4\n" << std::flush;
    std::thread* ThreadA = new std::thread(LUA_INTERPRETER_UNEDITABLE);
            //ThreadA->detach(); // TESTING - not working either..
    MyOutputFile << "setup5\n" << std::flush;
    {
        std::unique_lock<std::mutex> lk(m);
        MyOutputFile << "setup6\n" << std::flush;
        // ^^^ LAST THING TO BE PRINTED
        cv.wait(lk, [] {return (inLua == false); });
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

int main2()
{
    LUA_THREAD = setup();

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