#include "Aimbot.h"
#include "Glow.h"
#include "math.h"
#include "../utils/Logger.h"

void Aimbot::Aimbot() {
    if (!localPlayer)
        return;


    int state = inputSystem->Read<int>(inputBase + 0x3388);
    if (!state) {
        return;
    }
    QAngle viewAngle = process->Read<QAngle>(localPlayer + 0x20A8);
    //BreathCompensation(viewAngle, viewAngle);

    uintptr_t finalEntity = 0;

    Vector localOrigin = process->Read<Vector>(localPlayer + 0x12C);
    /*Vector localOrigin = process->Read<Vector>(localPlayer + 0x50);
    Vector abslocalOrigin = process->Read<Vector>(localPlayer + 0x4);
    Vector viewOffset = process->Read<Vector>(localPlayer + 0x30);
    Vector localAngles = process->Read<Vector>(localPlayer + 0x414);
    Vector pusherOrigin = process->Read<Vector>(localPlayer + 0x24);*/
    Vector eyepos = process->Read<Vector>(localPlayer + 0x3AA0);
    eyepos.x = localOrigin.x;
    eyepos.y = localOrigin.y;

    /*Logger::Log("Origin: (%f, %f, %f)\n", localOrigin.x, localOrigin.y, localOrigin.z);
    Logger::Log("absOrigin: (%f, %f, %f)\n", abslocalOrigin.x, abslocalOrigin.y, abslocalOrigin.z);
    Logger::Log("Offset: (%f, %f, %f)\n", viewOffset.x, viewOffset.y, viewOffset.z);
    Logger::Log("localAngles: (%f, %f, %f)\n", localAngles.x, localAngles.y, localAngles.z);
    Logger::Log("pusher: (%f, %f, %f)\n", pusherOrigin.x, pusherOrigin.y, pusherOrigin.z);
    Logger::Log("eyepos: (%f, %f, %f)\n", eyepos.x, eyepos.y, eyepos.z);*/
    //Vector pos = process->Read<Vector>(localPlayer + 0x3AA0);
    //Vector pos = GetBonePos(localPlayer, 12, localOrigin);
    //Vector pos = process->Read<Vector>(localPlayer + 0x3AA0);
    Vector pos = eyepos;

    int localTeam = process->Read<int>(localPlayer + 0x3E4);


    std::sort(entities.begin(), entities.end(), [viewAngle, &pos](const auto &a, const auto &b) {
        Vector a_pos = GetBonePos(a, 12, process->Read<Vector>(a + 0x12c));
        Vector b_pos = GetBonePos(b, 12, process->Read<Vector>(b + 0x12c));
        return Math::DistanceFOV(viewAngle, Math::CalcAngle(pos, a_pos), pos.DistTo(a_pos)) < Math::DistanceFOV(viewAngle, Math::CalcAngle(pos, b_pos), pos.DistTo(b_pos));
    });

    for (int ent = 0; ent < entities.size(); ent++) {
        uintptr_t entity = entities.at(ent);
        if (!entity || entity == localPlayer) {
            continue;
        }

        int teamnum = process->Read<int>(entity + 0x3E4);

        if (teamnum == localTeam) {
            continue;
        }

        int state = process->Read<int>(entity + 0x2368);

        if (state == 2)
            continue;
        finalEntity = entity;
        break;
    }
    if (!finalEntity)
        return;

    Vector origin = process->Read<Vector>(finalEntity + 0x12C);

    Vector enemyHeadPosition = GetBonePos(finalEntity, 12, origin);

    float dist = pos.DistTo(enemyHeadPosition);
    dist *= 0.01905f;

    uintptr_t weapon = GetActiveWeapon(localPlayer);

    if (!weapon) {
        return;
    }

    Nospread(weapon);

    float bulletVel = process->Read<float>(weapon + 0x1bac);
    if (bulletVel == 0.0f)
        return;

    bulletVel *= 0.01905f;

    Vector enemyVelocity = process->Read<Vector>(finalEntity + 0x120);
    Vector targetVelocity = enemyVelocity - process->Read<Vector>(localPlayer + 0x120);
    //targetVelocity *= 0.01905f;

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


    BreathCompensation(viewAngle, aimAngle);
    RecoilCompensation(aimAngle);
    aimAngle.Normalize();
    Math::Clamp(aimAngle);
    process->Write(localPlayer + 0x20B8, aimAngle);
    //process->Write(localPlayer + 0x20A8, aimAngle);
    //process->Write(localPlayer + 0x20BC, aimAngle.y);

    static float col[3] = {0.0f, 0.0f, 255.0f};
    Glow::GlowPlayer(finalEntity, col);
}

void Aimbot::RecoilCompensation(QAngle &angle) {
    QAngle aimpunch = process->Read<QAngle>(localPlayer + 0x2014);

    angle -= aimpunch;
}

void Aimbot::BreathCompensation(QAngle &viewAngle, QAngle &angle) {
    QAngle breath = process->Read<QAngle>(localPlayer + 0x20A8) - viewAngle;
    //Logger::Log("Breath: (%f, %f, %f)\n", breath.x, breath.y, breath.z);

    angle += breath * 2.0f;
}

void Aimbot::Nospread(uintptr_t weapon) {
    process->Write<float>(weapon + 0x1330, 0.0f);
    process->Write<float>(weapon + 0x1340, 0.0f);
}
