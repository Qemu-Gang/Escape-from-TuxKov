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

inline Vector GetBonePos(CBaseEntity &entity, int bone, const Vector &origin) {
    uintptr_t p_matrix = entity.boneMatrix;

    if (!p_matrix)
        return Vector();

    BoneMatrix matrix = process->Read<BoneMatrix>(p_matrix + bone * sizeof(BoneMatrix));
    Vector bonePos = Vector(matrix.x, matrix.y, matrix.z);
    bonePos += origin;

    return bonePos;
}

inline uintptr_t GetEntityById(ssize_t ent) {
    uintptr_t baseEntity = process->Read<uintptr_t>(entList);

    if (!baseEntity || !ent) {
        return (uintptr_t) NULL;
    }

    return process->Read<uintptr_t>(entList + (ent << 5));
}

inline uintptr_t GetActiveWeapon(CBaseEntity &entity) {
    uintptr_t weapon = entity.activeWeapon;
    if (!weapon)
        return 0;
    //Logger::Log("Weapon ptr: %p\n", (void*)weapon);

    weapon &= 0xFFFF;

    if (!weapon)
        return 0;
    //Logger::Log("ID: %i\n", weapon);

    return GetEntityById(weapon);
}

inline uintptr_t GetLocalPlayer() {
    uintptr_t localPlayerPtr = process->Read<uintptr_t>(apexBase + 0x22E3078);
    return localPlayerPtr;
}

inline uintptr_t GetLocalPlayerById() {
    localPlayerId = process->Read<int>(apexBase + 0x172EA34);

    for (int ent = 1; ent < 100; ent++) {
        uintptr_t entity = GetEntityById(ent);
        if (!entity)
            continue;

        int tmpId = process->Read<int>(entity + 0x8);
        if (tmpId == localPlayerId) {
            return entity;
        }
    }
    return 0;
}

inline bool IsPlayer(uintptr_t entity) {
    char buffer[20];
    VMemRead(&process->ctx->process, process->proc.dirBase, (uint64_t) buffer, process->Read<uintptr_t>(entity + 0x518), 20);
    if (buffer[0] == '\0')
        return false;

    return !strcmp(buffer, "player");
}

inline bool IsProp(uintptr_t entity) {
    static bool doOnce = false;
    if (!doOnce) {
        doOnce = true;
        int entityCount = process->Read<int>(apexBase + 0xC016EA0);
        for (int ent = 1; ent < entityCount; ent++) {
            char buffer[32];
            uintptr_t p_entity = GetEntityById(ent);
            if (!p_entity) continue;

            VMemRead(&process->ctx->process, process->proc.dirBase, (uint64_t) buffer, process->Read<uintptr_t>(p_entity + 0x518), 20);

            int mask = process->Read<int>(p_entity + 0x270);
            //Logger::Log("string: %s, id: %i, mask: %i\n", buffer, ent, mask);
        }

    }
    char buffer[20];

    VMemRead(&process->ctx->process, process->proc.dirBase, (uint64_t) buffer, process->Read<uintptr_t>(entity + 0x518), 20);
    if (buffer[0] == '\0')
        return false;
    bool state = !strcmp(buffer, "prop_survival");

    if (state) {
        //Logger::Log("%p is a prop\n", entity);
    }
    return state;
}