#include "Aimbot.h"
#include "Glow.h"
#include "../utils/Logger.h"

#include "../utils/Math.h"
#include "../utils/Wrappers.h"
#include "../utils/minitrace.h"

QAngle Aimbot::RecoilCompensation() {
    QAngle dynamic = localPlayer.aimPunch;

    return dynamic;
}

static void SwayCompensation(const QAngle &viewAngle, QAngle &angle) {
    QAngle dynamic = localPlayer.swayAngles;
    QAngle sway = dynamic - viewAngle;

    angle -= sway;
}

static void SpreadCompensation(uintptr_t weapon) {
    process->Write<float>(weapon + 0x1370, -1.0f);
    process->Write<float>(weapon + 0x1380, -1.0f);
}


float mag(Vector a)  // move this to vector class maybe?
{
    return std::sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
}

void Aimbot::Aimbot() {
    static Vector prevPosition[101];
    MTR_SCOPED_TRACE("Aimbot", "Run");

    if (!localPlayer)
        return;

    if (!(pressedKeys[BTN_SIDE]) && clientState.m_signonState == SIGNONSTATE_INGAMEAPEX) {
        // if we cannot run aimbot and we arent speedhacking reset fakelag
        //if (!(pressedKeys & KEY_ALT))
        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);
        return;
    }

    QAngle localAngles = localPlayer.viewAngles;

    Vector localEye = localPlayer.eyePos;

    uintptr_t weapon = GetActiveWeapon(localPlayer);

    float bulletVel = process->Read<float>(weapon + 0x1C24);

    if (bulletVel == 1.0f) { // 1.0f is fists.
        //Logger::Log("Not aimbotting on fists\n");
        return;
    }

    CBaseEntity *closestEnt = nullptr;
    float closest = __FLT_MAX__;
    float closestDist = __FLT_MAX__;
    Vector closestHeadPos;
    int closestID;

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
            closestID = entID;
        }
    }

    if (!closestEnt) {
        //Logger::Log("Couldn't find an ent to shoot\n");
        return;
    }

    Vector enemyVelocity = closestEnt->velocity;
    Vector enemyPosition = closestEnt->origin;
    float time = closestDist / bulletVel;

    closestHeadPos->x += time * enemyVelocity->x;
    closestHeadPos->y += time * enemyVelocity->y;

    if (!(closestEnt->GetFlags() & FL_ONGROUND)) {
        float dot = prevPosition[closestID].Dot(enemyPosition);
        float flyAngle = std::acos(dot / (mag(prevPosition[closestID]) * mag(enemyPosition)));

        Vector zAngle = prevPosition[closestID] - enemyPosition;

        if (zAngle->x == 0 && zAngle->y == 0 && enemyVelocity.Length() > 500.0f) { // ziplining vertically
            closestHeadPos->z += enemyVelocity->z * time + 375.0f * powf(time, 2.0f);
        } else if (enemyVelocity.Length() > 500.0f) { // ziplining or flying
            closestHeadPos->z += enemyVelocity->z * time * std::sin(flyAngle);
        } else { // jumping
            closestHeadPos->z += enemyVelocity->z * time * std::sin(flyAngle) - 375.0f * powf(time, 2.0f);
        }
    } else {
        closestHeadPos->z += enemyVelocity->z * time + 375.0f * powf(time, 2.0f);
    }
    for (size_t entID = 0; entID < validEntities.size(); entID++) {
        CBaseEntity &entity = entities[validEntities[entID]];
        if (!entity
            || entity == localPlayer
            || entity.GetTeamNum() == localPlayer.GetTeamNum()
            || entity.GetBleedoutState() != 0) {
            continue;
        }
        prevPosition[entID] = entity.origin;
    }

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

    //SpreadCompensation(weapon); // $wag
    SwayCompensation(localAngles, aimAngle);

    aimAngle.Normalize();
    Math::Clamp(aimAngle);

    localPlayer.viewAngles = aimAngle;
#endif

}
