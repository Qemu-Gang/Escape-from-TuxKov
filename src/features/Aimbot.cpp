#include "Aimbot.h"
#include "Glow.h"
#include "../utils/Logger.h"

#include "../utils/Math.h"
#include "../utils/Wrappers.h"
#include "../utils/minitrace.h"

/*QAngle Aimbot::RecoilCompensation() {
    QAngle dynamic = localPlayer.aimPunch;

    return dynamic;
}*/

static void RecoilCompensation(const QAngle &viewAngle, QAngle &angle) {
    QAngle recoil = localPlayer.aimPunch;

    angle -= recoil;
}
static void SwayCompensation(const QAngle &viewAngle, QAngle &angle) {
    QAngle dynamic = localPlayer.swayAngles;
    QAngle sway = dynamic - viewAngle;

    angle -= sway;
}

static void SpreadCompensation(uintptr_t weapon) {
    process->Write<float>(weapon + 0x13b0, -1.0f);
    process->Write<float>(weapon + 0x13c0, -1.0f);
}

void Smooth(QAngle &angle, QAngle &viewAngle, float val = 0.2f) { // Unused; needs update
    return;
    QAngle delta = angle - viewAngle;
    Math::Clamp(delta);

    if (delta.Length() < 0.1f)
        return;

    QAngle change;

    val = std::min(0.99f, val);
    change = delta.v * (1.0f - val);

    angle = viewAngle + change;
}

void Aimbot::Aimbot() {
    static Vector prevPosition[101];
    MTR_SCOPED_TRACE("Aimbot", "Run");

    static int lastEntity = -1;
    static uintptr_t plastEntity = 0;
    if (!localPlayer)
        return;

    if (!pressedKeys[KEY_LEFTALT] && clientState.m_signonState == SIGNONSTATE_INGAMEAPEX) {
        // if we cannot run aimbot and we arent speedhacking reset fakelag
        //if (!(pressedKeys & KEY_ALT))
        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);
        aimbotEntity = 0;
        lastEntity = -1;
        return;
    }

    QAngle localAngles = localPlayer.viewAngles;
    Vector localEye = localPlayer.eyePos;

    uintptr_t weapon = GetActiveWeapon(localPlayer);

    float bulletVel = process->Read<float>(weapon + 0x1C90);
    if (bulletVel == 1.0f) { // 1.0f is fists.
        //Logger::Log("Not aimbotting on fists\n");
        return;
    }

    CBaseEntity *closestEnt = nullptr;
    float closest = __FLT_MAX__;
    float closestDist = __FLT_MAX__;
    Vector closestHeadPos;
    int closestID;
    if (lastEntity > validEntities.size())
        lastEntity = -1;

    if (lastEntity != -1) {
        CBaseEntity &tmp = entities[validEntities[lastEntity]];

        if (tmp && plastEntity && tmp.GetBaseClass().address != plastEntity) {
            tmp.Update(plastEntity);
        }

        if (!tmp
            || tmp == localPlayer
            || tmp.GetTeamNum() == localPlayer.GetTeamNum()
            || tmp.GetBleedoutState() != 0
            || tmp.GetLifestate() != 0
            || !tmp.GetPlayerState()) {
            lastEntity = -1;
        }
    }

    for (size_t entID = 0; entID < validEntities.size(); entID++) {
        CBaseEntity &entity = entities[validEntities[entID]];
        if (!entity) {
            continue;
        }

        if (entity == localPlayer
            || entity.GetTeamNum() == localPlayer.GetTeamNum()
            || entity.GetBleedoutState() != 0
            || entity.GetLifestate() != 0
            || !entity.GetPlayerState()) {
            continue;
        }

        Vector headpos = GetBonePos(entity, 12, entity.origin);
        float dist = localEye.DistTo(headpos);
        float distFactor = Math::DistanceFOV(localAngles, QAngle(headpos - localEye), dist);
        //float distFactor = Math::AngleFOV(localAngles, QAngle(headpos - localEye));
        if (distFactor < closest && (lastEntity == -1 || entID == lastEntity)) {
            closest = distFactor;
            closestEnt = &entity;
            plastEntity = closestEnt->GetBaseClass().address;
            closestDist = dist;
            closestHeadPos = headpos;
            closestID = entID;
        }
    }

    if (lastEntity != -1) {
        CBaseEntity &tmp = entities[validEntities[lastEntity]];
        closestEnt = &tmp;
    }

    if (!closestEnt) {
        //Logger::Log("Couldn't find an ent to shoot\n");
        return;
    }
    lastEntity = closestID;

    aimbotEntity = closestEnt->GetBaseClass().address;

    Vector enemyVelocity = closestEnt->velocity;
    Vector finalVelocity = enemyVelocity;
    Vector finalHeadPos = closestHeadPos;

    float projectileGravityScale = process->Read<float>(weapon + 0x1C98);

    float time = closestDist / bulletVel;

    finalHeadPos->x += time * finalVelocity->x;

    finalHeadPos->y += time * finalVelocity->y;

    finalHeadPos->z += (enemyVelocity->z * time) + ((projectileGravityScale * 750.0f * 0.5 * powf(time, 2.0f)) / bulletVel); // Game prediction
    finalHeadPos->z -= 1.0f;

    for (size_t entID = 0; entID < validEntities.size(); entID++) {
        CBaseEntity &entity = entities[validEntities[entID]];
        if (!entity
            || entity == localPlayer
            || entity.GetTeamNum() == localPlayer.GetTeamNum()
            || entity.GetBleedoutState() != 0
            || entity.GetLifestate() != 0
            || !entity.GetPlayerState()) {
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
    QAngle aimAngle(finalHeadPos - localEye);

    if ((aimAngle->x == 0 && aimAngle->y == 0 && aimAngle->z == 0) || !aimAngle.IsValid()) {
        return;
    }

    //SpreadCompensation(weapon); // $wag
    SwayCompensation(localPlayer.viewAngles, aimAngle);

    aimAngle.Normalize();
    Math::Clamp(aimAngle);
    Smooth(aimAngle, localAngles);
    localPlayer.viewAngles = aimAngle;

    process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);

#endif

}
