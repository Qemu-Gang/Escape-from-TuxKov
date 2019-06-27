#include "Aimbot.h"
#include "Glow.h"
#include "../utils/Logger.h"

#include "../utils/Math.h"
#include "../utils/Wrappers.h"
#include "../utils/minitrace.h"

//#define SILENT_AIM

QAngle Aimbot::RecoilCompensation() {
    QAngle dynamic = localPlayer.swayAngles;

    return dynamic;
}

static void SwayCompensation(const QAngle &viewAngle, QAngle &angle) {
    QAngle dynamic = localPlayer.swayAngles;
    QAngle sway = dynamic - viewAngle;

    angle -= sway;
}

static void SwayCompensation(const QAngle &viewAngle, QAngle &angle, int commandNr) {
    QAngle dynamic = sway_history[commandNr];
    QAngle sway = dynamic - viewAngle;

    angle -= sway;
}

static void SpreadCompensation(uintptr_t weapon) {
    process->Write<float>(weapon + 0x1370, -1.0f);
    process->Write<float>(weapon + 0x1380, -1.0f);
}

void Aimbot::Aimbot() {
    MTR_SCOPED_TRACE("Aimbot", "Run");

    if (!localPlayer)
        return;
    if (!(pressedKeys & KEY_MOUSE4) && clientState.m_signonState == SIGNONSTATE_INGAMEAPEX) {
        // if we cannot run aimbot and we arent speedhacking reset fakelag
        //if (!(pressedKeys & KEY_ALT))
        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);

        return;
    }


    QAngle localAngles = localPlayer.viewAngles;

    Vector localEye = localPlayer.eyePos;
    //localEye->x = localOrigin->x;
    //localEye->y = localOrigin->y;
    uintptr_t weapon = GetActiveWeapon(localPlayer);

    float bulletVel = process->Read<float>(weapon + 0x1C0C);

    if (bulletVel == 1.0f) // 1.0f is fists.
        return;

    CBaseEntity *closestEnt = nullptr;
    float closest = __FLT_MAX__;
    float closestDist = __FLT_MAX__;
    Vector closestHeadPos;

    for (size_t entID = 0; entID < validEntities.size(); entID++) {
        CBaseEntity &entity = entities[validEntities[entID]];
        if (!entity
            || entity == localPlayer
            || entity.GetTeamNum() == localPlayer.GetTeamNum()
            || entity.GetBleedoutState() != 0) {
            continue;
        }

        Vector headpos = GetBonePos(entity, 12, entity.origin);
        float dist = localEye.DistTo(headpos);
        float distFactor = Math::DistanceFOV(localAngles, QAngle(headpos - localEye), dist);
        if (distFactor < closest) {
            closest = distFactor;
            closestEnt = &entity;
            closestDist = dist;
            closestHeadPos = headpos;
        }
    }

    if (!closestEnt) {
        Logger::Log("Couldn't find an ent to shoot\n");
        return;
    }

    Vector enemyVelocity = closestEnt->velocity;
    float xTime = closestDist / bulletVel;
    float yTime = xTime;
    Vector localVelocity = localPlayer.velocity;

    closestHeadPos->x += xTime * enemyVelocity->x - xTime * localVelocity->x; // velocity delta, fixes aim being off while moving
    closestHeadPos->y += yTime * enemyVelocity->y - xTime * localVelocity->y;
    closestHeadPos->z += (yTime * enemyVelocity->z + 375.0f * powf(xTime, 2.0f));
    closestHeadPos->z -= 1.0f;

//#define SILENT_AIM

#ifdef SILENT_AIM
    // if we can not fire, dont try to do silent aim (since the shot will be delayed, and aimbot will not work correctly - maybe account for tihs later?)
    if (process->Read<float>(weapon + 0x7B0) > globalVars.curtime)
        return;

    if (netChan.m_chokedCommands < 2) {
        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), std::numeric_limits<double>::max());
    }
    else {
        int32_t commandNr= process->Read<int32_t>(clientStateAddr + OFFSET_OF(&CClientState::m_lastUsedCommandNr));
        int32_t targetCommand = (commandNr - 1) % 300;

        CUserCmd userCmd = process->Read<CUserCmd>(userCmdArr + targetCommand * sizeof(CUserCmd));

        // manipulate usercmd here
        QAngle oldAngle = userCmd.m_viewAngles;

        QAngle aimAngle(closestHeadPos - userCmd.m_eyePos);
        if (aimAngle.IsZero() || !aimAngle.IsValid())
            return;

        //SwayCompensation(oldAngle, aimAngle, commandNr);

        aimAngle.Normalize();
        Math::Clamp(aimAngle);

        Math::CorrectMovement(&userCmd, oldAngle, userCmd.m_forwardmove, userCmd.m_sidemove);

        userCmd.m_viewAngles = aimAngle;
        userCmd.m_tickCount = globalVars.tickCount;
        userCmd.m_buttons |= IN_ATTACK;

        process->Write<CUserCmd>(userCmdArr + targetCommand * sizeof(CUserCmd), userCmd);
        process->Write<CUserCmd>(verifiedUserCmdArr + targetCommand * sizeof(CVerifiedUserCmd), userCmd);

        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);
    }

#else
    QAngle aimAngle(closestHeadPos - localEye);

    if ((aimAngle->x == 0 && aimAngle->y == 0 && aimAngle->z == 0) || !aimAngle.IsValid()) {
        return;
    }
    SpreadCompensation(weapon);
    SwayCompensation(localAngles, aimAngle);

    aimAngle.Normalize();
    Math::Clamp(aimAngle);

    localPlayer.viewAngles = aimAngle;
#endif


}
