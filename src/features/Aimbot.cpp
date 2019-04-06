#include "Aimbot.h"
#include "Glow.h"
#include "../utils/Logger.h"

#include "../utils/Math.h"
#include "../utils/Wrappers.h"
#include "../utils/minitrace.h"

//#define SILENT_AIM

static void RecoilCompensation(QAngle &angle) {
    angle -= localPlayer.aimPunch;
}

static void SwayCompensation(const QAngle &viewAngle, QAngle &angle) {
    QAngle dynamic = localPlayer.swayAngles;
    QAngle sway = dynamic - viewAngle;

    angle -= sway;
}

void Aimbot::Aimbot() {
    MTR_SCOPED_TRACE("Aimbot", "Run");

    if (!localPlayer)
        return;

    if (!(pressedKeys & KEY_MOUSE5) && clientState.m_signonState == SIGNONSTATE_INGAMEAPEX) {
        // if we cannot run aimbot and we arent speedhacking reset fakelag
        if (!(pressedKeys & KEY_ALT))
            process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);

        return;
    }

    QAngle localAngles = localPlayer.viewAngles;

    Vector localOrigin = localPlayer.origin;
    Vector localEye = localPlayer.eyePos;
    localEye->x = localOrigin->x;
    localEye->y = localOrigin->y;

    uintptr_t weapon = GetActiveWeapon(localPlayer);
    float bulletVel = process->Read<float>(weapon + 0x1BB4);

    if (bulletVel == 1.0f) // 1.0f is fists.
        return;

    CBaseEntity* closestEnt = nullptr;
    float closest = __FLT_MAX__;
    float closestDist = __FLT_MAX__;
    Vector closestHeadPos;

    for (size_t entID = 0; entID < validEntities.size(); entID++) {
        CBaseEntity &entity = entities[validEntities[entID]];
        if( !entity
            || entity == localPlayer
            || entity.GetTeamNum() == localPlayer.GetTeamNum()
            || entity.GetBleedoutState() != 0 ) {
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

    if (!closestEnt){
        Logger::Log("Couldn't find an ent to shoot\n");
        return;
    }

    Vector enemyVelocity = closestEnt->velocity;

    float xTime = closestDist / bulletVel;
    float yTime = xTime;

    closestHeadPos->x += xTime * enemyVelocity->x;
    closestHeadPos->y += yTime * enemyVelocity->y;
    closestHeadPos->z += yTime * enemyVelocity->z + 375.0f * powf(xTime, 2.0f);

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

        SwayCompensation(oldAngle, aimAngle);

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

    SwayCompensation(localAngles, aimAngle);

    aimAngle.Normalize();
    Math::Clamp(aimAngle);

    localPlayer.viewAngles = aimAngle;
#endif


}
