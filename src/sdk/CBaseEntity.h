#pragma once
#include "BaseStruct.h"

class CBaseEntity
{
public:
    char _pad[0x1B0];
    float glowR; // 1b0
    float glowG; // 1b4
    float glowB; // 1b8
    char _pad2[0xF8];
    float glowTimes1; // 2b0-2c8
    float glowTimes2; // 2b0-2c8
    float glowTimes3; // 2b0-2c8
    float glowTimes4; // 2b0-2c8
    float glowTimes5; // 2b0-2c8
    float glowTimes6; // 2b0-2c8
    char _pad3[0x10];
    float glowDistance; // 2DC
    char _pad4[0x10];
    int iGlowEnable; // 2f0
    char _pad5[0x8C];
    bool bGlowEnable; // 0x380

};
