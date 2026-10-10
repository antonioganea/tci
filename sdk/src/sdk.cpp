#include <windows.h>
#include <TCISDK/sdk/extension.h>

extern "C" int sdk_internal_entry();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            sdk_internal_entry();
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) int TCIExtension(const char *funcName, int argc, const char **argv) {
    return TCISDK::Extension::Execute(funcName, argc, argv);
}