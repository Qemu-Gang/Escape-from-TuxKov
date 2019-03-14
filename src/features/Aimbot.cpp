#include "Aimbot.h"
#include "Glow.h"
#include "../utils/Logger.h"

#include "../utils/Math.h"
#include "../utils/Wrappers.h"
#include "../utils/minitrace.h"

static void RecoilCompensation(QAngle &angle) {
    angle -= localPlayer.aimPunch;
}

static void SwayCompensation(QAngle &viewAngle, QAngle &angle) {
    QAngle dynamic = localPlayer.viewAngles;
    QAngle sway = dynamic - viewAngle;

    angle -= sway;
}

static void NoSpread(uintptr_t weapon) {
    process->Write<float>(weapon + 0x1330, 0.0f);
    process->Write<float>(weapon + 0x1340, 0.0f);
}


void Aimbot::Aimbot() {
    MTR_SCOPED_TRACE("Aimbot", "Run");

    if (!localPlayer)
        return;

    if( !inputSystem->Read<bool>(inputBase + 0xc7008) ) // mouse4 pressed down
        return;

    QAngle viewAngle = localPlayer.viewAngles;

    CBaseEntity* finalEntity = nullptr;

    Vector localOrigin = localPlayer.origin;

    Vector eyepos = localPlayer.eyePos;
    eyepos->x = localOrigin->x;
    eyepos->y = localOrigin->y;

    Vector pos = eyepos;

    int localTeam = localPlayer.teamNum;


    std::sort(sortedEntities.begin(), sortedEntities.end(), [viewAngle, &pos](const auto &a, const auto &b) {
        Vector a_pos = GetBonePos(entities[a], 12, entities[a].origin);
        Vector b_pos = GetBonePos(entities[b], 12, entities[b].origin);
        return Math::DistanceFOV(viewAngle, QAngle(a_pos - pos), pos.DistTo(a_pos)) < Math::DistanceFOV(viewAngle, QAngle(b_pos - pos), pos.DistTo(b_pos));
    });

    for (size_t ent = 0; ent < sortedEntities.size(); ent++) {
        CBaseEntity& entity = entities[sortedEntities[ent]];
        if (!entity || entity == localPlayer) {
            continue;
        }

        if (entity.teamNum == localTeam) {
            continue;
        }

        int lifeState = entity.lifeState;

        if (lifeState != 0)
            continue;
        finalEntity = &entity;
        break;
    }
    if (!finalEntity)
        return;

    Vector enemyHeadPosition = GetBonePos(*finalEntity, 12, finalEntity->origin);

    float dist = pos.DistTo(enemyHeadPosition);
    dist *= 0.01905f;

    uintptr_t weapon = GetActiveWeapon(localPlayer);

    if (!weapon) {
        return;
    }

    //NoSpread(weapon);

    float bulletVel = process->Read<float>(weapon + 0x1bac);
    if (bulletVel == 0.0f)
        return;

    bulletVel *= 0.01905f;

    Vector enemyVelocity = finalEntity->velocity;
    Vector targetVelocity = enemyVelocity;

    float xTime = dist / bulletVel;
    float yTime = xTime;

    enemyHeadPosition->x += xTime * targetVelocity->x;
    enemyHeadPosition->y += yTime * targetVelocity->y;
    enemyHeadPosition->z += yTime * targetVelocity->z + 375.0f * powf(xTime, 2.0f);

    QAngle aimAngle(enemyHeadPosition - pos);

    aimAngle.Normalize();
    Math::Clamp(aimAngle);

    //int lifeState = process->Read<int>(finalEntity + 0x718);

    if ((aimAngle->x == 0 && aimAngle->y == 0 && aimAngle->z == 0) || !aimAngle.IsValid()) {
        return;
    }

    SwayCompensation(viewAngle, aimAngle);
    RecoilCompensation(aimAngle);
    aimAngle.Normalize();
    Math::Clamp(aimAngle);

    localPlayer.swayAngles = aimAngle;
}
