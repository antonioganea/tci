extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

#include "lua-api.h"
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

void registerLuaApiFunctions(lua_State* L) {
    const luaL_Reg* func;
    for (func = luaApiFunctions; func->func; func++) {
        lua_pushcfunction(state, func->func);
        lua_setglobal(state, func->name);
    }
}

char bootPath[512];

void createLuaState(const char* path) {
    state = luaL_newstate();

    // Make standard libraries available in the Lua object
    // luaL_openlibs(state); <--- this crashes ( because of luaopen_package )
    openCustomLuaLibs(state);

    registerLuaApiFunctions(state);

    strcpy_s(bootPath, path);
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

void JustBooted()
{
    int res = luaL_dofile(state, bootPath);
    switch (res)
    {
        case LUA_ERRSYNTAX:
            //puts("[Lua] Error executing line ( syntax ) !");
            break;
        case LUA_ERRMEM:
            //puts("[Lua] Error executing line ( memory ) !");
            break;
        default:
        {
            //int res = lua_pcall(state, 0, LUA_MULTRET, 0);
            if (res != LUA_OK)
            {
                print_error_console(state);
                break;
            }
        }
    }
}

void freeLuaStateInternally() {
    lua_close(state);
    ResetHandlers();
}