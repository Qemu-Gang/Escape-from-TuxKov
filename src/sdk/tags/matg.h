#pragma once

#include "TagBase.h"

/* I think this file ended up being a waste of time lol */

enum class DamageResist_t : uint8_t
{
    UNCHANGED = 0,
    PERCENT_10,
    PERCENT_50,
    PERCENT_90,
    PERCENT_100,
    PERCENT_110,
    PERCENT_150,
    PERCENT_200,
    PERCENT_300,
    PERCENT_500,
    PERCENT_1000,
    PERCENT_2000,
    INVULNERABLE,
};

enum class HealthMultiplier_t : uint8_t
{
    UNCHANGED = 0,
    PERCENT_0,
    PERCENT_100,
    PERCENT_150,
    PERCENT_200,
    PERCENT_300,
    PERCENT_400,
};

enum class HealthRechargeRate_t : uint8_t
{
    UNCHANGED = 0,
    PERCENT_NEGATIVE_25,
    PERCENT_NEGATIVE_10,
    PERCENT_NEGATIVE_5,
    PERCENT_0,
    PERCENT_10,
    PERCENT_25,
    PERCENT_50,
    PERCENT_75,
    PERCENT_90,
    PERCENT_100,
    PERCENT_110,
    PERCENT_125,
    PERCENT_150,
    PERCENT_200,
};

enum class ShieldMultiplier_t : uint8_t
{
    UNCHANGED = 0,
    NO_SHIELD,
    NORMAL,
    OVERSHIELD_1_5X,
    OVERSHIELD_2X,
    OVERSHIELD_3X,
    OVERSHIELD_4X
};

enum class ShieldRechargeRate_t : uint8_t
{
    UNCHANGED = 0,
    PERCENT_NEGATIVE_25,
    PERCENT_NEGATIVE_10,
    PERCENT_NEGATIVE_5,
    PERCENT_0,
    PERCENT_10,
    PERCENT_25,
    PERCENT_50,
    PERCENT_75,
    PERCENT_90,
    PERCENT_100,
    PERCENT_110,
    PERCENT_125,
    PERCENT_150,
    PERCENT_200,
};

enum class OnOffState : uint8_t
{
    UNCHANGED = 0,
    DISABLED = 1,
    ENABLED = 2
};

typedef OnOffState HeadShotImmunity_t;
typedef OnOffState AssassinationImmunity_t;
typedef OnOffState DeathImmunity_t;
typedef OnOffState GrenadeRegeneration_t;
typedef OnOffState WeaponPickup_t;
typedef OnOffState EquipmentUsage_t;
typedef OnOffState EquipmentDrop_t;
typedef OnOffState InfiniteEquipment_t;
typedef OnOffState DoubleJump_t;

enum class InfiniteAmmo_t
{
    UNCHANGED = 0,
    DISABLED = 1,
    ENABLED = 2,
    BOTTOMLESS_CLIP = 3,
};

enum class ShieldVampirism_t : uint8_t
{
    UNCHANGED = 0,
    DISABLED,
    PERCENT_10,
    PERCENT_25,
    PERCENT_50,
    PERCENT_100
};

enum class DamageModifier_t : uint8_t
{
    UNCHANGED = 0,
    PERCENT_0,
    PERCENT_25,
    PERCENT_50,
    PERCENT_75,
    PERCENT_90,
    PERCENT_100,
    PERCENT_110,
    PERCENT_125,
    PERCENT_150,
    PERCENT_200,
    PERCENT_300,
    INSTANT_KILL
};

enum class GrenadeCount_t : uint8_t
{
    UNCHANGED = 0,
    MAP_DEFAULT,
    NONE,
    FRAG_1,
    FRAG_2,
    FRAG_3,
    FRAG_4,
    PLASMA_1,
    PLASMA_2,
    PLASMA_3,
    PLASMA_4,
    BOTH_1,
    BOTH_2,
    BOTH_3,
    BOTH_4
};

enum class PlayerSpeed_t : uint8_t
{
    UNCHANGED = 0,
    PERCENT_0,
    PERCENT_25,
    PERCENT_50,
    PERCENT_75,
    PERCENT_90,
    PERCENT_100,
    PERCENT_110,
    PERCENT_120,
    PERCENT_130,
    PERCENT_140,
    PERCENT_150,
    PERCENT_160,
    PERCENT_170,
    PERCENT_180,
    PERCENT_190,
    PERCENT_200,
    PERCENT_300,
};

enum class PlayerGravity_t : uint8_t
{
    UNCHANGED = 0,
    PERCENT_50,
    PERCENT_75,
    PERCENT_100,
    PERCENT_150,
    PERCENT_200,
    PERCENT_250,
    PERCENT_300,
    PERCENT_350,
    PERCENT_400,
    PERCENT_450,
    PERCENT_500,
    PERCENT_550,
    PERCENT_600
};

enum class VehicleUse_t : uint8_t
{
    UNCHANGED = 0,
    NONE = 1,
    PASSENGER_ONLY = 2,
    DRIVER_ONLY = 3,
    GUNNER_ONLY = 4,
    NO_PASSENGER = 5,
    NO_DRIVER = 6,
    NO_GUNNER = 7,
    FULL_USE = 8
};


class ShieldsAndHealthSubTag
{
public:
    DamageResist_t dmgResist;
    HealthMultiplier_t hpMultiplier;
    HealthRechargeRate_t hpRechargeRate;
    ShieldMultiplier_t shieldMultiplier;
    ShieldRechargeRate_t shieldRechargeRate;
    ShieldRechargeRate_t shieldRechargeRate2;
    HeadShotImmunity_t headShotImmunity;
    ShieldVampirism_t shieldLifeSteal;
    AssassinationImmunity_t assassinationImmunity;
    DeathImmunity_t deathImmunity;
};

class WeaponsAndDamageSubTag
{
public:
    DamageModifier_t dmgModifier;
    DamageModifier_t dmgMeleeModifier;
    GrenadeRegeneration_t grenadeRegeneration;
    WeaponPickup_t weaponPickup;
    GrenadeCount_t grenadeCount;
    InfiniteAmmo_t infiniteAmmo;
    EquipmentUsage_t equipmentUsage;
    EquipmentDrop_t equipmentDrop;
    InfiniteEquipment_t infiniteEquipment;
    char _pad[3];
    uint32_t primaryWeapon_StringID;
    uint32_t secondaryWeapon_StringID;
    uint32_t equipment_StringID;
};

class MovementSubTag
{
public:
    PlayerSpeed_t playerSpeed;
    PlayerGravity_t playerGravity;
    VehicleUse_t vehicleUse;
    DoubleJump_t doubleJump;
    int32_t jumpHeight; // Default is -1. values:[1-400]
};

class PlayerTraitDefaultsSubTag
{
public:
    SubTagListRef<ShieldsAndHealthSubTag> shieldsAndHealth;
    SubTagListRef<WeaponsAndDamageSubTag> weaponsAndDamage;
    SubTagListRef<MovementSubTag> movement;
    SubTagListRef<UnknownTag> appearance;
    SubTagListRef<UnknownTag> hud;
};

class matg
{
public:
    char pad_0000[176]; //0x0000
    SubTagListRef<UnknownTag> havokCleanupResources; //0x00B0
    SubTagListRef<UnknownTag> soundGlobals; //0x00BC
    SubTagListRef<UnknownTag> aiGlobals; //0x00C8
    TagRef aiGlobalsRef; //0x00D4
    SubTagListRef<UnknownTag> damageTable; //0x00E4
    TagRef N000018DA; //0x00F0
    SubTagListRef<UnknownTag> sounds; //0x0100
    SubTagListRef<UnknownTag> camera; //0x010C
    SubTagListRef<UnknownTag> thumbstickDeadzones; //0x0118
    SubTagListRef<UnknownTag> playerControl; //0x0124
    SubTagListRef<UnknownTag> playerTraitDefaults; //0x0130
    SubTagListRef<UnknownTag> difficulty; //0x013C
    SubTagListRef<UnknownTag> N000018B8; //0x0148
    SubTagListRef<UnknownTag> grenades; //0x0154
    SubTagListRef<UnknownTag> softBarrierProperties; //0x0160
    char pad_016C[12]; //0x016C
    SubTagListRef<UnknownTag> interfaceTags; //0x0178
    char pad_0184[24]; //0x0184
    SubTagListRef<UnknownTag> playerInfo; //0x019C
    SubTagListRef<UnknownTag> playerRepresentation; //0x01A8
    SubTagListRef<UnknownTag> fallingDamage; //0x01B4
    SubTagListRef<UnknownTag> N00001903; //0x01C0
    SubTagListRef<UnknownTag> materials; //0x01CC
    SubTagListRef<UnknownTag> playerColors; //0x01D8
    SubTagListRef<UnknownTag> emblemColors; //0x01E4
    SubTagListRef<UnknownTag> visorColors; //0x01F0
    float eliteArmorShineR; //0x01FC
    float eliteArmorShineG; //0x0200
    float eliteArmorShineB; //0x0204
    float eliteArmorColorR; //0x0208
    float eliteArmorColorG; //0x020C
    float eliteArmorColorB; //0x0210
    SubTagListRef<UnknownTag> forgeColors; //0x0214
    TagRef gameEngineGlobals; //0x0220
    TagRef multiplayerGlobals; //0x0230
    TagRef survivalGlobals; //0x0240
    TagRef objectTypeList; //0x0250
    SubTagListRef<UnknownTag> cinematicAnchors; //0x0260
    SubTagListRef<UnknownTag> metagameGlobals; //0x026C
    TagRef medalGlobals; //0x0278

    /* There is more here, but it's mostly B.S. localization... region... raster settings.. blah blah */
};