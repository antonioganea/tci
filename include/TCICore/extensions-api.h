#pragma once

#include <Windows.h>
#include <minwindef.h>
#include "utils.h"

#define MAX_EXT_NAME 256
#define MAX_CACHE_SIZE 64
#define EXTENSION_ENTRYPOINT_NAME "TCIExtension"

typedef void (*ExtensionFunctionType)(const char *, int, const char **);

typedef struct {
    char extName[MAX_EXT_NAME];
    HMODULE hModule;
    bool isOccupied;
} ExtensionCacheEntry;

struct CrashExtensionContext{
    std::string extName;
    std::string funcName;
};

SDKAPI void LoadExtension(const char *extName);
SDKAPI void FreeExtension(const char *extName);
SDKAPI void CallExtension(const char *extName, const char *funcName, int argc, const char **argv);
