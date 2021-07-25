#include "norecoil.h"

#include "../globals.h"


void NoRecoil::ApplyNoRecoil( ) {
    if( !localPlayerAddr )
        return;

    Player localplayer = process->Read<Player>( localPlayerAddr );
    ProceduralWeaponAnimation weaponAnimation = process->Read<ProceduralWeaponAnimation>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation );
    Firing fire = process->Read<Firing>( (uintptr_t)weaponAnimation.m_pFiring );

    ShotEffector shotEffector = process->Read<ShotEffector>( (uintptr_t)weaponAnimation.m_pShooting );
    shotEffector.Intensity = 0.0f;
    process->Write<ShotEffector>( (uintptr_t)weaponAnimation.m_pShooting, shotEffector );

    BreathEffector breathEffector = process->Read<BreathEffector>( (uintptr_t)weaponAnimation.m_pBreath );
    breathEffector.Intensity = 0.0f;
    process->Write<BreathEffector>( (uintptr_t)weaponAnimation.m_pBreath, breathEffector );


    weaponAnimation.mask = 1;
    process->Write<ProceduralWeaponAnimation>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation, weaponAnimation );
    //process->Write<int32_t>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation + offsetof(ProceduralWeaponAnimation, Mask), 1 );


    //process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, floater1), 1.0f );
    process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, floater2), 0.0f ); // spread
    //process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, floater3), 1.0f );
    //fire.bulletSpeedMultiplier = 5.0f;
    process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, bulletSpeedMultiplier), 4.9f );
}