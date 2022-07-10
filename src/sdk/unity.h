#pragma once

#include "tarkov.h"

#include <cstdint>

struct GameWorldWrapper
{
    char pad_0[0x28];
    LOCALGAMEWORLD* localgameworld;
};

struct unity_transform
{
    char pad_0[0x10];
    void* transform;
};

struct game_object_wrapper
{
    char pad_0[0x18];
    void* gameObject;
};

struct mono_object
{
    char pad_0000[0x30];
    game_object_wrapper *pObjectClass;
    char pad_0038[0x10];
    int16_t Unk;
    char pad_004A[6];
    int32_t Layer;
    int16_t Tag;
    char pad_0056[10];
    char *objectname;
};

struct mono_object_wrapper
{
    char pad_0[0x8];
    mono_object_wrapper* next;
    mono_object* object;
};

struct GlobalObjectManager
{
    void* shitlistLast;
    void* shitlistFirst; // 0x8
    mono_object_wrapper *taggedLast; // 0x10
    mono_object_wrapper *taggedFirst; // 0x18
    mono_object_wrapper *activeLast;  // 0x20
    mono_object_wrapper *activeFirst; // 0x28
};


namespace Unity
{
    void PrintGOMObjects( bool tagged );
    void PrintPlayerList();
    void PrintItemStats();
    Vector3D GetBonePosition( uintptr_t playerAddr, int boneID );
    uintptr_t GetObjectPtrByName( const char *objname, bool tagged );
    uintptr_t GetWorldPtr();
    bool World2Screen( const Vector3D &world, Vector2D *screen );
}


enum Bones : int
{
    HumanBase = 0,
    HumanPelvis = 14,
    HumanLThigh1 = 15,
    HumanLThigh2 = 16,
    HumanLCalf = 17,
    HumanLFoot = 18,
    HumanLToe = 19,
    HumanRThigh1 = 20,
    HumanRThigh2 = 21,
    HumanRCalf = 22,
    HumanRFoot = 23,
    HumanRToe = 24,
    HumanSpine1 = 29,
    HumanSpine2 = 36,
    HumanSpine3 = 37,
    HumanLCollarbone = 89,
    HumanLUpperarm = 90,
    HumanLForearm1 = 91,
    HumanLForearm2 = 92,
    HumanLForearm3 = 93,
    HumanLPalm = 94,
    HumanRCollarbone = 110,
    HumanRUpperarm = 111,
    HumanRForearm1 = 112,
    HumanRForearm2 = 113,
    HumanRForearm3 = 114,
    HumanRPalm = 115,
    HumanNeck = 132,
    HumanHead = 133
};