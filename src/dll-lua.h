#pragma once

// Alternatives on other operating systems :
// ucontext_t - POSIX alternative
// <boost/context/fiber.hpp> - Boost.Context

enum class DayZServerCommands
{
    Nothing,
    OnUpdate,
    OnKilled,
    JustBooted,
    OnCommand = 2103,
    OnUpdatePass = 4912,
    OnGenericCommand = 5522
};

extern LPVOID g_gameFiber, g_luaFiber;

void goInLua();
void goOutOfLua();

void freeLuaStateInternally();
void startLuaFiber(const char* path);
void shutdownLuaFiber();