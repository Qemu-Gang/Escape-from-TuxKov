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
    Signature(g_tag_addresses, "[48 89 05 *?? ?? ?? ??] 49 8B C8 8B", MODNAME),
    Signature(currentMapAddr, "[0F 11 05 *?? ?? ?? ??] E8 ?? ?? ?? ?? 0F 10 05", MODNAME),
    Signature(currentMapStringIDTable, "08 00 00 00 [48 8B 0D *?? ?? ?? ??] E8", MODNAME),
};