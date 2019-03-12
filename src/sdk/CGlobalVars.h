#pragma once

class CGlobalVars
{
public:
    double realtime;
    int32_t framecount; //0x0008
    float absoluteframetime; //0x000C
    float curtime; //0x0010
    float N00000047; //0x0014
    float N00000018; //0x0018
    float N0000004A; //0x001C
    float N00000019; //0x0020
    float N0000004D; //0x0024
    float N0000001A; //0x0028
    char pad_002C[4]; //0x002C
    float N0000001B; //0x0030
    char pad_0034[12]; //0x0034
    int32_t tickCount; //0x0040
    float intervalPerTick; //0x0044
}; //Size: 0x0048
