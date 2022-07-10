#include "aimbot.h"

#include "../globals.h"
#include "../sdk/Vector.h"
#include "../utils/Math.h"

#include <csignal>

Vector3D CalculateAngle(const Vector3D &local, const Vector3D &enemy) {
    Vector3D delta = local - enemy;
    //delta.y -= 180.0f;
    //delta.NormalizeAngles();

    float len = delta.Length();

    Vector3D ret;
    ret.y = asinf(delta.y / len ) * RAD2DEG;
    ret.x = -atan2f(delta.x, -delta.z) * RAD2DEG;

    return ret;
}

static MovementContext movement;
void Aimbot::Aim() {
    if( !pressedKeys[KEY_1] )
        return;

    LOCALGAMEWORLD gameworld = process->Read<LOCALGAMEWORLD>( gameWorldAddr );

    Array playerList = process->Read<Array>( (uintptr_t)gameworld.m_pPlayerList );

    for( int i = 0; i < playerList.Count; i++ ){
        uintptr_t playerAddr = process->Read<uintptr_t>((uintptr_t) playerList.m_pList + (0x20 + (i * 8)));
        Player player = process->Read<Player>( playerAddr );
        if( player.m_pLocalPlayerChecker ){
            localPlayerAddr = playerAddr;
        }
    }

    Player localPlayer;
    localPlayer = process->Read<Player>( localPlayerAddr );
    movement = process->Read<MovementContext>( (uintptr_t)localPlayer.m_pMovementContext );
    //std::raise(SIGINT);
    // Garbage data.
    if( playerList.Count > 100 || playerList.Count < 0 )
        return;

    MovementContext localMovement = process->Read<MovementContext>((uintptr_t)localPlayer.m_pMovementContext);

    QAngle localView;
    localView.x = localMovement.ViewAngles.x;
    localView.y = localMovement.ViewAngles.y;

    volatile float fov;
    float bestFov = 999.0f;
    uintptr_t chosenPlayer = 0;
    QAngle chosenPlayerAngle;

    // Get closest player in fov
    for( int i = 0; i < playerList.Count; i++ ) {
        uintptr_t playerAddr = process->Read<uintptr_t>((uintptr_t) playerList.m_pList + (0x20 + (i * 8)));

        if (!playerAddr)
            continue;

        Vector3D headPos = Unity::GetBonePosition( playerAddr, Bones::HumanHead );
        QAngle aimAngle = CalculateAngle( localPlayerHead, headPos );

        fov = Math::AngleFOV( localView, aimAngle );
        Logger::Log("Fov(%f)\n", fov);
        if( fov < bestFov ){
            Logger::Log("FOUND NEW BEST FOV! REEEE!\n");
            bestFov = fov;
            chosenPlayer = playerAddr;
            chosenPlayerAngle = aimAngle;
        }
    }

    if( !chosenPlayer || bestFov > 180.0f ){
        Logger::Log("No suitable aimbot target.\n");
        Logger::Log("best fov(%f)\n", bestFov);
        return;
    }

    Logger::Log("Writing angle..(%f/%f)\n", chosenPlayerAngle.x, chosenPlayerAngle.y);
    localMovement.ViewAngles.x = chosenPlayerAngle.x;
    localMovement.ViewAngles.y = chosenPlayerAngle.y;
    process->Write<MovementContext>( (uintptr_t)localPlayer.m_pMovementContext, localMovement );
}