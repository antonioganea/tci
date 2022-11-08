// dllmain.cpp : Defines the entry point for the DLL application.

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <thread>

#include "bridge.h"
#include "detour.h"
#include "utils.h"

DWORD WINAPI HackThread(HMODULE hModule) {

    SetupDataBridge(); // blocks thread until bridge file is available for reading
    CreateDetour();

    return 0;
}

#include "gui.h"

DWORD WINAPI GuiThread(HMODULE hModule) {
    gui::CreateHWindow("TCI", "TCI Menu Class");
    gui::CreateDevice();
    gui::CreateImGui();

    while (gui::exit) {

        gui::BeginRender();
        gui::Render();
        gui::EndRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }

    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)GuiThread, hModule, 0, nullptr));
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        // Unsure if this is needed.
        //cleanup();
        break;
    }
    return TRUE;
}