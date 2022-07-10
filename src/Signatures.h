#pragma once

#include <vector>

#include "globals.h"

struct Signature
{
    uintptr_t* result;
    const char* module;
    const char* pattern;

    template<typename T>
    Signature(T& ref, const char* p, const char* m)
    : result((uintptr_t*)&ref), module(m), pattern(p) {}
};

inline std::vector<Signature> signatures = {
    // Example - Signature(entList, "[48 8D 15 *?? ?? ?? ??] 48 C1 E1 05 C1", MODNAME),
    Signature(gomAddr, "[48 8B 05 **?? ?? ?? ??] 66 83 F9 05", MODNAME),
};