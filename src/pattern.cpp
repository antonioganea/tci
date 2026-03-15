#include "pattern.h"
#include <vector>
#include <Psapi.h>

namespace Pattern {

uintptr_t GetModuleBase(const wchar_t* moduleName) {
    if (!moduleName) {
        // If no module name provided, return current module (DLL)
        return reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
    }
    
    // Try Unicode first
    HMODULE hModule = GetModuleHandleW(moduleName);
    if (hModule) {
        return reinterpret_cast<uintptr_t>(hModule);
    }
    
    // Fallback to ANSI (convert wide string to ANSI)
    char ansiName[MAX_PATH];
    int len = WideCharToMultiByte(CP_ACP, 0, moduleName, -1, ansiName, MAX_PATH, nullptr, nullptr);
    if (len > 0) {
        hModule = GetModuleHandleA(ansiName);
        if (hModule) {
            return reinterpret_cast<uintptr_t>(hModule);
        }
    }
    
    return 0;
}

size_t GetModuleSize(HMODULE module) {
    if (!module) return 0;
    
    MODULEINFO modInfo;
    if (GetModuleInformation(GetCurrentProcess(), module, &modInfo, sizeof(modInfo))) {
        return modInfo.SizeOfImage;
    }
    return 0;
}

// Convert signature string to bytes
// Format: "48 8B 05 ?? ?? ?? ?? 48 85 C0"
static std::vector<int> ParseSignature(const char* signature) {
    std::vector<int> bytes;
    const char* current = signature;
    
    while (*current) {
        if (*current == ' ') {
            current++;
            continue;
        }
        if (*current == '?') {
            bytes.push_back(-1); // Wildcard
            current++;
            if (*current == '?') current++;
            continue;
        }
        // Parse hex byte
        char byte[3] = { current[0], current[1], '\0' };
        bytes.push_back(static_cast<int>(strtol(byte, nullptr, 16)));
        current += 2;
    }
    
    return bytes;
}

uintptr_t PatternScan(HMODULE module, const char* signature) {
    if (!module) return 0;
    
    auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
    auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(
        reinterpret_cast<uint8_t*>(module) + dosHeader->e_lfanew);
    
    uintptr_t start = reinterpret_cast<uintptr_t>(module);
    size_t size = ntHeaders->OptionalHeader.SizeOfImage;
    
    auto pattern = ParseSignature(signature);
    auto patternSize = pattern.size();
    auto scanBytes = reinterpret_cast<uint8_t*>(start);
    
    for (size_t i = 0; i < size - patternSize; i++) {
        bool found = true;
        for (size_t j = 0; j < patternSize; j++) {
            if (pattern[j] != -1 && scanBytes[i + j] != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return start + i;
        }
    }
    
    return 0;
}

uintptr_t PatternScanStartingAt(uint64_t start, uint64_t size, const char* signature) {
    auto pattern = ParseSignature(signature);
    auto patternSize = pattern.size();
    auto scanBytes = reinterpret_cast<uint8_t*>(start);
    
    for (size_t i = 0; i < size - patternSize; i++) {
        bool found = true;
        for (size_t j = 0; j < patternSize; j++) {
            if (pattern[j] != -1 && scanBytes[i + j] != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return start + i;
        }
    }
    
    return 0;
}

uintptr_t PatternScanEx(uintptr_t start, size_t size, const char* pattern, const char* mask) {
    size_t patternLen = strlen(mask);
    auto scanBytes = reinterpret_cast<uint8_t*>(start);
    auto patternBytes = reinterpret_cast<const uint8_t*>(pattern);
    
    for (size_t i = 0; i < size - patternLen; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (mask[j] != '?' && scanBytes[i + j] != patternBytes[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return start + i;
        }
    }
    
    return 0;
}


uintptr_t PatternScanAllMemory(const char* signature) {
    auto pattern = ParseSignature(signature);
    size_t patternSize = pattern.size();
    if (patternSize == 0) return 0;

    MEMORY_BASIC_INFORMATION mbi{};
    uintptr_t address = 0;

    while (VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
        // Only scan committed, readable memory
        // Skip PAGE_NOACCESS and PAGE_GUARD pages to avoid access violations
        bool isCommitted = (mbi.State == MEM_COMMIT);
        bool isReadable  = (mbi.Protect & (
            PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ |
            PAGE_EXECUTE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_WRITECOPY
        )) != 0;
        bool isGuarded = (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0;

        if (isCommitted && isReadable && !isGuarded) {
            auto regionStart = reinterpret_cast<uint8_t*>(mbi.BaseAddress);
            size_t regionSize = mbi.RegionSize;

            if (regionSize >= patternSize) {
                for (size_t i = 0; i <= regionSize - patternSize; i++) {
                    bool found = true;
                    for (size_t j = 0; j < patternSize; j++) {
                        if (pattern[j] != -1 && regionStart[i + j] != static_cast<uint8_t>(pattern[j])) {
                            found = false;
                            break;
                        }
                    }
                    if (found) {
                        return reinterpret_cast<uintptr_t>(regionStart + i);
                    }
                }
            }
        }

        // Advance to next region; guard against infinite loop on overflow
        uintptr_t next = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        if (next <= address) break;
        address = next;
    }

    return 0;
}

} // namespace Pattern
