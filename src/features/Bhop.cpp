#include "Bhop.h"
#include "../utils/Wrappers.h"
#include "../utils/Math.h"

void Bhop::Bhop( CBaseEntity &localplayer ) {
    if( !(pressedKeys[KEY_SPACE]) ){
        return;
    }

    if( localplayer.GetFlags() & FL_ONGROUND ){
        process->Write<int>( forceJump, 6 );
    }
}

void Bhop::Strafe() {
    return; // TODO: Reverse InputAddr sig

    if (netChan.m_chokedCommands < 2) {
        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), std::numeric_limits<double>::max());
    }
    else {
        int32_t commandNr = process->Read<int32_t>(clientStateAddr + OFFSET_OF(&CClientState::m_lastUsedCommandNr));
        int32_t targetCommand = (commandNr - 1) % 300;

        CUserCmd cmd = process->Read<CUserCmd>(userCmdArr + targetCommand * sizeof(CUserCmd));
        static bool leftRight;

        static QAngle oldAngle;
        QAngle newAngle = cmd.m_viewAngles;

        QAngle delta = oldAngle - newAngle;
        float mousedx = delta->x / 0.022f; // m_pitch
        float mousedy = delta->y / 0.022f; // m_yaw

        bool inMove = cmd.m_buttons & IN_FORWARD || cmd.m_buttons & IN_BACK || cmd.m_buttons & IN_MOVELEFT || cmd.m_buttons & IN_MOVERIGHT;
        Vector velocity = localPlayer.velocity;
        if (cmd.m_buttons & IN_FORWARD && velocity.Length() <= 50.0f)
            cmd.m_forwardmove = 250.0f;

        float yaw_change = 0.0f;
        if (velocity.Length() > 50.f)
            yaw_change = 30.0f * fabsf(30.0f / velocity.Length());


        if (cmd.m_buttons & IN_ATTACK)
            yaw_change = 0.0f;

        QAngle viewAngles = localPlayer.viewAngles;

        if (!(localPlayer.GetFlags() & FL_ONGROUND) && !inMove) {
            if (leftRight || mousedx > 1) {
                viewAngles->y += yaw_change;
                cmd.m_sidemove = 250.0f;
            } else if (!leftRight || mousedx < 1) {
                viewAngles->y -= yaw_change;
                cmd.m_sidemove = -250.0f;
            }

            leftRight = !leftRight;
        }

        viewAngles.Normalize();

        Math::Clamp(viewAngles);

        Math::CorrectMovement(&cmd, viewAngles, cmd.m_forwardmove, cmd.m_sidemove);

        oldAngle = cmd.m_viewAngles;

        cmd.m_viewAngles = viewAngles;

        process->Write<CUserCmd>(userCmdArr + targetCommand * sizeof(CUserCmd), cmd);
        process->Write<CUserCmd>(verifiedUserCmdArr + targetCommand * sizeof(CVerifiedUserCmd), cmd);

        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);
    }
}
