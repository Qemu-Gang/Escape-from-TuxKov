#include "Aimbot.h"
#include "Glow.h"
#include "../utils/Logger.h"

#include "../utils/Math.h"
#include "../utils/Wrappers.h"
#include "../utils/minitrace.h"

static void RecoilCompensation(QAngle &angle) {
    angle -= localPlayer.aimPunch;
}

static void SwayCompensation(const QAngle &viewAngle, QAngle &angle) {
    QAngle dynamic = localPlayer.swayAngles;
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

    if (!(pressedKeys & KEY_MOUSE4))
        return;

    sendpacket = true; // want to send packets when aiming

    QAngle localAngles = localPlayer.viewAngles;

    Vector localOrigin = localPlayer.origin;
    Vector localEye = localPlayer.eyePos;
    localEye->x = localOrigin->x;
    localEye->y = localOrigin->y;

    CBaseEntity* closestEnt = nullptr;
    float closest = __FLT_MAX__;
    float closestDist = __FLT_MAX__;
    Vector closestHeadPos;

    for (size_t entID = 0; entID < validEntities.size(); entID++) {
        CBaseEntity &entity = entities[validEntities[entID]];
        if( !entity
            || entity == localPlayer
            || entity.teamNum == localPlayer.teamNum
            || entity.lifeState != 0 ) {
            continue;
        }

        Vector headpos = GetBonePos(entity, 12, entity.origin);
        float dist = localEye.DistTo(headpos);
        float distFactor = Math::DistanceFOV(localAngles, QAngle( headpos - localEye ), dist);
        if( distFactor < closest ){
            closest = distFactor;
            closestEnt = &entity;
            closestDist = dist;
            closestHeadPos = headpos;
        }
    }

    if (!closestEnt)
        return;

    uintptr_t weapon = GetActiveWeapon(localPlayer);

    if (!weapon) { // TODO: this gun check doesn't work
        return;
    }

    NoSpread(weapon);

    float bulletVel = process->Read<float>(weapon + 0x1bac);
    if (bulletVel == 0.0f)
        return;

    Vector enemyVelocity = closestEnt->velocity;

    float xTime = closestDist / bulletVel;
    float yTime = xTime;

    closestHeadPos->x += xTime * enemyVelocity->x;
    closestHeadPos->y += yTime * enemyVelocity->y;
    closestHeadPos->z += yTime * enemyVelocity->z + 375.0f * powf(xTime, 2.0f);

    QAngle aimAngle(closestHeadPos - localEye);

    aimAngle.Normalize();
    Math::Clamp(aimAngle);

    if ((aimAngle->x == 0 && aimAngle->y == 0 && aimAngle->z == 0) || !aimAngle.IsValid()) {
        return;
    }

    SwayCompensation(localAngles, aimAngle);

    aimAngle.Normalize();
    Math::Clamp(aimAngle);

    localPlayer.viewAngles = aimAngle;
}
