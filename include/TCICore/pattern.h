#pragma once
#include <cstdint>
#include <initializer_list>
#include <Windows.h>

namespace Pattern {

// Pattern scan in a module
uintptr_t PatternScan(HMODULE module, const char* signature);

uintptr_t PatternScanStartingAt(uint64_t start, uint64_t size, const char* signature);

// Pattern scan with mask
uintptr_t PatternScanEx(uintptr_t start, size_t size, const char* pattern, const char* mask);

uintptr_t PatternScanAllMemory(const char* signature);

// Read memory with pointer chain
template<typename T>
T Read(uintptr_t address) {
    if (!address) return T{};
    return *reinterpret_cast<T*>(address);
}

// Read pointer chain: Read<uintptr_t>(base, {0x10, 0x20, 0x30})
template<typename T>
T ReadChain(uintptr_t base, std::initializer_list<uintptr_t> offsets) {
    uintptr_t addr = base;
    for (auto offset : offsets) {
        if (!addr) return T{};
        addr = *reinterpret_cast<uintptr_t*>(addr);
        if (!addr) return T{};
        addr += offset;
    }
    return *reinterpret_cast<T*>(addr);
}

// Get module base address
uintptr_t GetModuleBase(const wchar_t* moduleName = nullptr);

// Get module size
size_t GetModuleSize(HMODULE module);

} // namespace Pattern
