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
    Signature(globalVars, "[4C 8B 15 **?? ?? ?? ??] 88", MODNAME),
    Signature(netTime, "[F2 0F 58 0D *?? ?? ?? ??] 66 0F 2F C1 77", MODNAME),
    Signature(nextCmdTime, "[F2 0F 10 05 *?? ?? ?? ??] F2 0F 58 0D", MODNAME),
    Signature(signonState, "[83 3D *?? ?? ?? ?? ??] 0F B6 DA", MODNAME),
    Signature(netChannel, "[48 8B 1D **?? ?? ?? ??] 48 8D 05 ?? ?? ?? ?? 48 89 ?? ?? ?? 8B 05", MODNAME),
};
