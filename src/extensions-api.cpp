#include <TCICore/extensions-api.h>
#include <TCICore/tci-api.h>
#include <excpt.h>
#include <string>
#include <thread>
#include <vector>
#include <setjmp.h>

static thread_local jmp_buf g_ExtensionJumpBuffer;
static thread_local bool g_InExtensionCall = false;
thread_local CrashExtensionContext g_CrashExtensionContext;

static ExtensionCacheEntry g_ExtensionCache[MAX_CACHE_SIZE] = {};

void LoadExtension(const char *extName) {
    if (!extName) return;
    int firstEmptyIdx = -1;

    for (int i = 0; i < MAX_CACHE_SIZE; i++) {
        if (!g_ExtensionCache[i].isOccupied) {
            if (firstEmptyIdx == -1) {
                firstEmptyIdx = i;
            }
            continue;
        }

        if (_stricmp(g_ExtensionCache[i].extName, extName) == 0) {
            return;
        }
    }

    if (firstEmptyIdx != -1) {
        HMODULE hModule = LoadLibraryA(extName);
        if (hModule != nullptr) {
            strcpy_s(g_ExtensionCache[firstEmptyIdx].extName, MAX_EXT_NAME, extName);
            g_ExtensionCache[firstEmptyIdx].hModule = hModule;
            g_ExtensionCache[firstEmptyIdx].isOccupied = true;
        }
    } else {
        ConsoleMessage("Cache size limit exceeded");
    }
}

void FreeExtension(const char *extName) {
    if (!extName) return;

    for (int i = 0; i < MAX_CACHE_SIZE; i++) {
        if (g_ExtensionCache[i].isOccupied && _stricmp(g_ExtensionCache[i].extName, extName) == 0) {
            if (g_ExtensionCache[i].hModule != nullptr) {
                FreeLibrary(g_ExtensionCache[i].hModule);
            }
            g_ExtensionCache[i].hModule = nullptr;
            g_ExtensionCache[i].extName[0] = '\0';
            g_ExtensionCache[i].isOccupied = false;
        }
    }
}

void CallExtension(const char *extName, const char *funcName, int argc, const char **argv) {
    if (!extName) return;

    // ConsoleMessage((std::string("In C++: extName: ") + extName).c_str());

    for (int i = 0; i < MAX_CACHE_SIZE; i++) {
        if (g_ExtensionCache[i].isOccupied && _stricmp(g_ExtensionCache[i].extName, extName) == 0) {
            HMODULE hModule = g_ExtensionCache[i].hModule;
            if (hModule == nullptr) continue;

            ExtensionFunctionType func = (ExtensionFunctionType)GetProcAddress(hModule, EXTENSION_ENTRYPOINT_NAME);
            if (!func) {
                ConsoleMessage(
                    (std::string("Failed to find function in ")
                        + extName + " . Function name: " + funcName).c_str());
                return;
            }

            __try {
                func(funcName, argc, argv);
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                DWORD errCode = GetExceptionCode();

                ConsoleMessage((std::string("Extension Critical Error: Name: ")
                    + extName + " ; Func: "
                    + funcName + " ; Code: "
                    + std::to_string(errCode)).c_str());

                FreeExtension(extName);
            }

            return;
        }
    }

    ConsoleMessage("Failed to find DLL in cache. Please, load it before calling");
}
