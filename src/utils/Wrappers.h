#pragma once

#include "../globals.h"

#include "../sdk/CBaseEntity.h"
#include "../sdk/Vector.h"
#include "../sdk/QAngle.h"
#include "Logger.h"

struct BoneMatrix {
    char __buff_0x00[0xC];//0x00
    float x;//0xC
    char __buff_0x10[0xC];//0x10
    float y;//0x1c
    char __buff_0x20[0xC];//0x20
    float z;//0x2c
};

inline Vector GetBonePos(CBaseEntity &entity, int bone, const Vector &rootpos) {
    uintptr_t p_matrix = entity.boneMatrix;

    if (!p_matrix)
        return Vector();

    BoneMatrix matrix = process->Read<BoneMatrix>(p_matrix + bone * sizeof(BoneMatrix));
    Vector returnW = Vector(matrix.x, matrix.y, matrix.z);
    returnW += rootpos;
    //fprintf(out, "Bone: (%f, %f, %f)\n", returnW.x, returnW.y, returnW.z);

    return returnW;
}

inline uintptr_t GetEntityById(ssize_t ent) {
    uintptr_t baseEntity = process->Read<uintptr_t>(entList);

    if (!baseEntity || !ent) {
        return (uintptr_t)NULL;
    }

    return process->Read<uintptr_t>(entList + (ent << 5));
}

inline uintptr_t GetActiveWeapon(CBaseEntity &entity) {
    uintptr_t weapon = entity.activeWeapon;
    if(!weapon)
        return 0;
    //Logger::Log("Weapon ptr: %p\n", (void*)weapon);

    weapon &= 0xFFFF;

    if(!weapon)
        return 0;
    //Logger::Log("ID: %i\n", weapon);

    return GetEntityById(weapon);
}

inline uintptr_t GetLocalPlayer() {
    int add = process->Read<int>(apexBase + 0x1747EFC);

    for (int ent = 1; ent < 100; ent++) {
        uintptr_t entity = GetEntityById(ent);
        if (!entity)
            continue;

        int tmpId = process->Read<int>(entity + 0x8);
        if (tmpId == add) {
            return entity;
        }
    }
    return 0;
}
