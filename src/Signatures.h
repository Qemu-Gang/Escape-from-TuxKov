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
    // Xref "Script_GetTitan" it should look like a cvar register, go into function right under string, then the 2nd function in that
    Signature(entList, "[48 8D 15 *?? ?? ?? ??] 48 C1 E1 05 C1", MODNAME),
    Signature(globalVarsAddr, "F3 0F 59 CF E8 ?? ?? ?? ?? [48 8B 05 **?? ?? ?? ??] 48 85 C0 74", MODNAME), // xref for "VoiceCommVolume" - should be in same block at the bottom.
    Signature(inputAddr, "[48 8D 0D *?? ?? ?? ??] FF 90 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 3B", MODNAME), // xref for "OnRenderStart->CViewRender::SetUpView" - should be a few lines below that string
    Signature(clientStateAddr, "[48 8D 05 *?? ?? ?? ??] 48 03 C8 E8 ?? ?? ?? ?? E9", MODNAME),
    Signature(timescale, "[F3 0F 11 ?? *?? ?? ?? 01] F3 0F 10 ?? ?? ?? ?? 01 F3 0F 11 ?? ?? ?? ?? 01 44 89", MODNAME),
    Signature(forceJump, "[8b 0d *?? ?? ?? ??] c6 05 ?? ?? ?? ?? 00 f6 c1 03 75 ?? 80 3d ?? ?? ?? ?? 00 74 ?? 83 cb 02", MODNAME),
};