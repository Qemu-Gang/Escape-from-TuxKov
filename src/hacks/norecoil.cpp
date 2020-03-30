#include "norecoil.h"

#include "../globals.h"


void NoRecoil::ApplyNoRecoil( ) {
    if( !localPlayerAddr )
        return;

    Player localplayer = process->Read<Player>( localPlayerAddr );
    ProceduralWeaponAnimation weaponAnimation = process->Read<ProceduralWeaponAnimation>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation );
    ShotEffector shotEffector = process->Read<ShotEffector>( (uintptr_t)weaponAnimation.m_pShooting );
    shotEffector.Intensity = 0.0f;
    process->Write<ShotEffector>( (uintptr_t)weaponAnimation.m_pShooting, shotEffector );

    BreathEffector breathEffector = process->Read<BreathEffector>( (uintptr_t)weaponAnimation.m_pBreath );
    breathEffector.Intensity = 0.0f;
    process->Write<BreathEffector>( (uintptr_t)weaponAnimation.m_pBreath, breathEffector );
}