#pragma once

struct Signature
{
    uintptr_t* result;
    const char* module;
    const char* pattern;

    template<typename T>
    Signature(T& ref, const char* p, const char* m)
    : result((uintptr_t*)&ref), module(m), pattern(p) {}
};

inline const Signature signatures[] = {
    Signature(entList, "[48 8D 05 *?? ?? ?? ??] 48 C1 E1 05 48 03 C8 0F B7 05 ?? ?? ?? ?? 39 41 08 75 51", MODNAME),
    Signature(globalVarsAddr, "[4C 8B 15 **?? ?? ?? ??] 88", MODNAME),
    Signature(inputAddr, "[48 8D 0D *?? ?? ?? ??] 44 0F B6 CB 0F 28 D6 41 8B D6", MODNAME),
    Signature(clientStateAddr, "[33 D2 48 8D 0D *? ? ? ?] E8 ? ? ? ? EB 2E", MODNAME),
    Signature(timescale, "[F3 0F 11 ?? *?? ?? ?? 01] F3 0F 10 ?? ?? ?? ?? 01 F3 0F 11 ?? ?? ?? ?? 01 44 89", MODNAME),
};