#ifndef APE_EX_WRAPPERS_H
#define APE_EX_WRAPPERS_H

#include "../utils.h"

#include "../sdk/Vector.h"
#include "../sdk/QAngle.h"

struct BoneMatrix {
    char __buff_0x00[0xC];//0x00
    float x;//0xC
    char __buff_0x10[0xC];//0x10
    float y;//0x1c
    char __buff_0x20[0xC];//0x20
    float z;//0x2c
};

inline Vector GetBonePos(uintptr_t entity, int bone, Vector rootpos) {

    uintptr_t p_matrix = process->Read<uintptr_t>(entity + 0xED8);

    if (!p_matrix)
        return Vector();

    BoneMatrix matrix = process->Read<BoneMatrix>(p_matrix + bone * sizeof(BoneMatrix));
    Vector returnW = Vector(matrix.x, matrix.y, matrix.z);
    returnW += rootpos;
    //fprintf(out, "Bone: (%f, %f, %f)\n", returnW.x, returnW.y, returnW.z);

    return returnW;
}

inline uintptr_t GetEntityById(int ent) {
    uintptr_t entList = apexLegendsBase + 0x1F6CAB8;
    uintptr_t baseEntity = process->Read<uintptr_t>(entList);

    if (!baseEntity || !ent) {
        return NULL;
    }

    return process->Read<uintptr_t>(entList + (ent << 5));
}

inline uintptr_t GetActiveWeapon(uintptr_t entity, FILE* out) {
    uintptr_t weapon = process->Read<uintptr_t>(entity + 0x1634);
    if(!weapon)
        return 0;

    weapon &= 0xFFFF;

    if(!weapon)
        return 0;
    return GetEntityById(weapon);
}

inline uintptr_t GetLocalPlayer() {
    int add = process->Read<int>(apexLegendsBase + 0x172263C);

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
#endif  // APE_EX_WRAPPERS_H
