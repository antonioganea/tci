#pragma once

#include <string>

#define SDKAPI extern "C" __declspec(dllexport)
#define SDKAPI_STRUCT __declspec(dllexport)

bool fileExistsTest(const std::wstring& name);

struct TTT {
    unsigned int low, high;
};

union TConv {
    TTT t;
    unsigned long long int big;
};