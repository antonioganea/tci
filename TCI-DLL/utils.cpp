#include "utils.h"

#include <fstream>

bool fileExistsTest(const std::wstring& name) {
    std::ifstream f(name.c_str());
    return f.good();
}