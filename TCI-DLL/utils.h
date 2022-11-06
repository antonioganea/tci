#pragma once

#include <string>

bool fileExistsTest(const std::wstring& name);

struct TTT {
    unsigned int low, high;
};

union TConv {
    TTT t;
    unsigned long long int big;
};