#include "Aimbot.h"
#include "Glow.h"
#include "math.h"
void Aimbot::Aimbot(FILE* out) {
    if(!localPlayer)
        return;

    int state = inputSystem->Read<int>(inputSystemBase + 0x8030);
    if (!state) {
        return;
    }

    uintptr_t finalEntity = 0;
    QAngle viewAngle = process->Read<QAngle>(localPlayer + 0x20B8);
    BreathCompensation(out, viewAngle);

    Vector localOrigin = process->Read<Vector>(localPlayer + 0x12C);
    //Vector localOrigin = process->Read<Vector>(localPlayer + 0x50);
    //Vector viewOffset = process->Read<Vector>(localPlayer + 0x30);
    //Vector pos = process->Read<Vector>(localPlayer + 0x3AA0);
    Vector pos = GetBonePos(localPlayer, 12, localOrigin);

    int localTeam = process->Read<int>(localPlayer + 0x3E4);


    float bestFov = __FLT_MAX__;
    for (int ent = 0; ent < entities.size(); ent++) {

        uintptr_t entity = entities.at(ent);
        if (!entity || entity == localPlayer) {
            continue;
        }

        int teamnum = process->Read<int>(entity + 0x3E4);

        if (teamnum == localTeam) {
            continue;
        }
        int lifeState = process->Read<int>(entity + 0x718);

        if (lifeState)
            continue;

        Vector origin = process->Read<Vector>(entity + 0x12C);

        Vector enemyHeadPosition = GetBonePos(entity, 10, origin);
        if(enemyHeadPosition.x == 0.0f && enemyHeadPosition.y == 0.0f && enemyHeadPosition.z == 0.0f)
            continue;

        float distance = pos.DistTo(enemyHeadPosition);

        QAngle aimAngle = Math::CalcAngle(pos, enemyHeadPosition);
        if((aimAngle.x == 0.0f && aimAngle.y == 0.0f && aimAngle.z == 0.0f)|| isnan(aimAngle.x) || isnan(aimAngle.y) || isnan(aimAngle.z) )
            continue;

        float fov = Math::DistanceFOV(viewAngle, aimAngle, distance);
        if (fov < bestFov) {
            finalEntity = entity;
            bestFov = fov;
        }

    }

    if(!finalEntity)
        return;

    Vector origin = process->Read<Vector>(finalEntity + 0x12C);

    Vector enemyHeadPosition = GetBonePos(finalEntity, 10, origin);
    float dist = pos.DistTo(enemyHeadPosition);
    dist *= 0.01905f;
    dist *= 1.1f;
    uintptr_t weapon = GetActiveWeapon(localPlayer, out);
    if (!weapon) {
        return;
    }

    float bulletVel = process->Read<float>(weapon + 0x1bac);
    if(bulletVel == 0.0f)
        return;

    bulletVel *= 0.01905f;

    Vector targetVelocity = process->Read<Vector>(finalEntity + 0x120);
    targetVelocity *= 0.01905f;

    //float interval_per_tick = process->Read<float>(0x1713CA8 + 0x44);



    float xTime = dist / bulletVel;
    float yTime = xTime;

    enemyHeadPosition.x += xTime * targetVelocity.x;
    enemyHeadPosition.y += yTime * targetVelocity.y;
    enemyHeadPosition.z += 375.0f * powf(xTime, 2.0f);

    QAngle aimAngle = Math::CalcAngle(pos, enemyHeadPosition);

    aimAngle.Normalize();
    Math::Clamp(aimAngle);

    //int lifeState = process->Read<int>(finalEntity + 0x718);

    if ((aimAngle.x == 0 && aimAngle.y == 0 && aimAngle.z == 0) || isnan(aimAngle.x) || isnan(aimAngle.y) || isnan(aimAngle.z)) { // TODO: Put this in QAngle function
        return;
    }

    RecoilCompensation(aimAngle);


    aimAngle.Normalize();
    Math::Clamp(aimAngle);
    process->Write(localPlayer + 0x20B8, aimAngle.x);
    process->Write(localPlayer + 0x20BC, aimAngle.y);

    static float col[3] = {0.0f, 0.0f, 255.0f};
    Glow::GlowPlayer(finalEntity, col);
}

void Aimbot::RecoilCompensation(QAngle &angle) {
    QAngle aimpunch = process->Read<QAngle>(localPlayer + 0x2014);

    angle -= aimpunch;
}

void Aimbot::BreathCompensation(FILE* out, QAngle &angle) {
    QAngle breath = process->Read<QAngle>(localPlayer + 0x20A8) - angle;
    Math::Clamp(breath);
    angle -= breath;
    //fprintf(out, "Breath: (%f, %f, %f)\n", breath.x, breath.y, breath.z);
}
