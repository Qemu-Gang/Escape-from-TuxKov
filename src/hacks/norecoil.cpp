#include "norecoil.h"

#include "../globals.h"
#include "../utils/Logger.h"


void NoRecoil::ApplyNoRecoil( ) {
    if( !localPlayerAddr )
        return;

    Player localplayer = process->Read<Player>( localPlayerAddr );
    ProceduralWeaponAnimation weaponAnimation = process->Read<ProceduralWeaponAnimation>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation );

    //HandsController handsController = process->Read<HandsController>( (uintptr_t)localplayer.m_pHandsController );
    //WeaponItem weaponItem = process->Read<WeaponItem>( (uintptr_t)handsController.m_pWeaponItem );
    //ItemTemplate itemTemplate = process->Read<ItemTemplate>( (uintptr_t) weaponItem.m_BackingField);
    //itemTemplate.m_allowFeed = false;
    //itemTemplate.m_allowJam = false;
    //itemTemplate.m_allowMisfire = false;
    //itemTemplate.m_allowSlide = false;
    //process->Write<ItemTemplate>( (uintptr_t) weaponItem.m_BackingField, itemTemplate );

    //Logger::Log("proceduralWeaponAnim (%p)\n", localplayer.m_pProceduralWeaponAnimation);
    ShotEffector shotEffector = process->Read<ShotEffector>( (uintptr_t)weaponAnimation.m_pShooting );
    shotEffector.recoilHori = 0.0f;
    shotEffector.recoilVert = 0.0f;
    process->Write<ShotEffector>( (uintptr_t)weaponAnimation.m_pShooting, shotEffector );

    BreathEffector breathEffector = process->Read<BreathEffector>( (uintptr_t)weaponAnimation.m_pBreath );
    breathEffector.Intensity = 0.0f;
    process->Write<BreathEffector>( (uintptr_t)weaponAnimation.m_pBreath, breathEffector );

    weaponAnimation.mask = 1;
    process->Write<ProceduralWeaponAnimation>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation, weaponAnimation );

    FirearmController fire = process->Read<FirearmController>( (uintptr_t)weaponAnimation.m_pFiring );

    WeaponItem weaponItem = process->Read<WeaponItem>( (uintptr_t)fire.m_pItem );
    WeaponTemplate weaponTemplate = process->Read<WeaponTemplate>( (uintptr_t)weaponItem.m_BackingField );
    if (weaponTemplate.m_allowFeed)
    {
        weaponTemplate.m_allowSlide = false;
        weaponTemplate.m_allowMisfire = false;
        weaponTemplate.m_allowJam = false;
        weaponTemplate.m_allowFeed = false;
        process->Write<WeaponTemplate>( (uintptr_t)weaponItem.m_BackingField, weaponTemplate );
    }

    fire.m_hipInaccuracy = 0.0f;
    //fire.m_weaponLn = 0.1f; // buggy
    process->Write<FirearmController>( (uintptr_t)weaponAnimation.m_pFiring, fire );
    //process->Write<int32_t>( (uintptr_t)localplayer.m_pProceduralWeaponAnimation + offsetof(ProceduralWeaponAnimation, Mask), 1 );


    //process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, floater1), 1.0f );
    //process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, floater2), 0.0f ); // spread
    //process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, floater3), 1.0f );
    //fire.bulletSpeedMultiplier = 5.0f;
    //process->Write<float>( (uintptr_t)weaponAnimation.m_pFiring + offsetof(Firing, bulletSpeedMultiplier), 4.9f );
}