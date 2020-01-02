#pragma once

#include "TagBase.h"


/* From halo assembly weap.xml
 * Credits:
 * -DeToX-
 * -Chrisco93-
 * ManBearPig_06
 * Lord Zedd
 * Mobius118
 */
enum ObjectType_t : int16_t
{
    BIPED,
    VEHICLE,
    WEAPON,
    EQUIPMENT,
    TERMINAL,
    PROJECTILE,
    SCENERY,
    MACHINE,
    CONTROL,
    SOUNDSCENERY,
    CRATE,
    CREATURE,
    GIANT,
    EFFECTSCENARY
};

enum GeneralFlags_t : uint32_t
{
    DOES_NOT_CAST_SHADOW = (1 << 0),
    SEARCH_CARDINAL_DIRECTION_LIGHTMAPS_ON_FAILURE = (1 << 1),
    PRESERVES_INITIAL_DAMAGE_OWNER = (1 << 2),
    NOT_A_PATHFINDING_OBSTACLE = (1 << 3),
    EXTENSION_OF_PARENT = (1 << 4),
    DOES_NOT_CAUSE_COLLISION_DAMAGE = (1 << 5),
    EARLY_MOVER = (1 << 6),
    EARLY_MOVER_LOCALIZED_PHYSICS = (1 << 7),
    USE_STATIC_MASSIVE_LIGHTMAP_SAMPLE = (1 << 8),
    OBJECT_SCALES_ATTACHMENTS = (1 << 9),
    INHERITS_PLAYERS_APPEARANCE = (1 << 10),
    NON_PHYSICAL_IN_MAP_EDITOR = (1 << 11),
    OBJECT_ALWAYS_ON_CEILING = (1 << 12),
    SAMPLE_ENVIRONMENT_LIGHTING_ONLY_IGNORE_OBJECT_LIGHTING = (1 << 13),
    EFFECTS_CREATED_BY_THIS_OBJECT_DO_NOT_SPAWN_OBJECTS_IN_MP= (1 << 14),
    DOES_NOT_COLLIDE_WITH_CAMERA = (1 << 15),
    FORCE_COLLIDE_WITH_CAMERA = (1 << 16),
    DAMAGE_NOT_BLOCKED_BY_OBSTRUCTIONS = (1 << 17),
    DOES_NOT_DAMAGE_BREAKABLE_SURFACES = (1 << 18),
};

enum class WeaponFlags_t : uint32_t
{
    VERTICAL_HEAT_DISPLAY = ( 1 << 0 ),
    MUTEX_TRIGGERS = ( 1 << 1 ),
    AUTOATTACK_ON_BUMP = ( 1 << 2 ),
    MUST_BE_READIED = ( 1 << 3 ),
    DOESNT_COUNT_TOWARDS_MAX = ( 1 << 4 ),
    AIM_ASSIST_ONLY_WHEN_ZOOMED = ( 1 << 5 ),
    PREVENT_GRENADE_THROWS = ( 1 << 6 ),
    MUST_BE_PICKED_UP = ( 1 << 7 ),
    HOLDS_TRIGGERS_WHEN_DROPPED = ( 1 << 8 ),
    PREVENT_MELEE = ( 1 << 9 ),
    DETONATE_ON_DROP = ( 1 << 10 ),
    CANNOT_FIRE_WHEN_MAX_AGED = ( 1 << 11 ),
    SECONDARY_OVERRIDES_GRENADES = ( 1 << 12 ),
    DOES_NOT_DEPOWER_ACTIVE_CAMO_MP = ( 1 << 13 ),
    ENABLES_NIGHTVISION = ( 1 << 14 ),
    AI_USE_WEAPON_MELEE_DAMAGE = ( 1 << 15 ),
    FORCE_NO_BINOS = ( 1 << 16 ),
    LOOP_FP_FIRING_ANIM = ( 1 << 17 ),
    PREVENT_SPRINTING = ( 1 << 18 ),
    CANNOT_FIRE_WHILE_BOOSTING = ( 1 << 19 ),
    PREVENTS_DRIVING = ( 1 << 20 ),
    THIRD_PERSON_CAMERA = ( 1 << 21 ),
    CAN_DUAL_WIELD = ( 1 << 22 ),
    CAN_ONLY_DUAL_WIELD = ( 1 << 23 ),
    MELEE_ONLY = ( 1 << 24 ),
    CANT_FIRE_IF_PARENT_DEAD = ( 1 << 25 ),
    WEAPON_AGES_WITH_KILLS = ( 1 << 26 ),
    WEAPON_USES_OLD_DUAL_FIRE_ERROR_CODE = ( 1 << 27 ),
    MELEE_ON_TRIGGER = ( 1 << 28 ),
    CANNOT_BE_USED_BY_PLAYER = ( 1 << 29 ),

};
enum class LightMapSize_t : int16_t
{
    DEFAULT,
    NEVER,
    ALWAYS,
    UNKNOWN
};

enum SweetenerSize_t : int8_t
{
    SMALL,
    MEDIUM,
    LARGE
};

enum class WaterDensity_t : int8_t
{
    DEFAULT,
    LEAST,
    SOME,
    EQUAL,
    MORE,
    MORETHANMORE,
    LOTSMORE
};

class MagazineSubTag
{
public:
    uint32_t flags; //0x0000
    char pad_0004[2]; //0x0004
    int16_t startingAmmo; //0x0006
    int16_t maxRoundsTotal; //0x0008
    int16_t magazineSize; //0x000A
    int16_t maxReserveAmmo; //0x000C
    char pad_000E[2]; //0x000E
    float reloadTime; //0x0010
    int16_t roundsPerReload; //0x0014 // how many to stuff in the magazine per reload
    int16_t N00001843; //0x0016
    float chamberTime; //0x0018
    char pad_001C[24]; //0x001C
    TagRef reloadEffect; //0x0034
    TagRef reloadDamageEffect; //0x0044
    TagRef chamberingEffect; //0x0054
    TagRef chamberingDamageEffect; //0x0064
    SubTagListRef<UnknownTag> magazineEquipment; //0x0074
}; //Size: 0x0080

enum class TriggerButton_t : int16_t
{
    RIGHT_TRIGGER,
    LEFT_TRIGGER,
    MELEE_ATTACK,
    AUTOMATED_FIRE,
    RIGHT_BUMPER
};

enum class TriggerBehavior_t : int16_t
{
    SPEW,
    LATCH,
    LATCH_AUTOFIRE,
    CHARGE,
    LATCH_ZOOM,
    LATCH_ROCKETLAUNCHER,
    SPEW_CHARGE,
    CHARGE_MELEE,
};

enum class TriggerAction_t : int16_t
{
    FIRE,
    CHARGE,
    TRACK,
    FIRE_OTHER
};

enum class OverChargeAction_t : int8_t
{
    NONE,
    EXPLODE,
    DISCHARGE
};

class TriggerSubTag
{
public:
    uint32_t flags; //0x0000
    TriggerButton_t buttonUsed; //0x0004
    TriggerBehavior_t behavior; //0x0006
    int16_t primaryBarrel; //0x0008
    int16_t secondaryBarrel; //0x000A
    int16_t prediction; //0x000C
    char pad_000E[2]; //0x000E
    float autoFireTime; //0x0010
    float autoFireThrow; //0x0014
    TriggerAction_t secondaryAction; //0x0018
    TriggerAction_t primaryAction; //0x001A
    float chargingTime; //0x001C
    float chargedTime; //0x0020
    OverChargeAction_t overchargeAction; //0x0024
    int8_t chargeProjectiles; //0x0025
    int16_t chargeFlags; //0x0026
    float chargedIllumination; //0x0028
    TagRef chargingEffect; //0x002C
    TagRef chargingDamageEffect; //0x003C
    TagRef chargingResponse; //0x004C
    float chargingAgeDegeneration; //0x005C
    TagRef dischargingEffect; //0x0060
    TagRef dischargingDamageEffect; //0x0070
    SubTagListRef<UnknownTag> chargeLevels; //0x0080
}; //Size: 0x008C

enum class BarrelFlags_t : uint32_t
{
    TRACKS_FIRED_PROJECTILE = ( 1 << 0 ),
    RANDOM_FIRING_EFFECTS = ( 1 << 1 ),
    CAN_FIRE_WITH_PARTIAL_AMMO = ( 1 << 2 ),
    PROJECTILES_USE_WEAPON_ORIGIN = ( 1 << 3 ),
    EJECTS_DURING_CHAMBER = ( 1 << 4 ),
    USE_ERROR_WHEN_UNZOOMED = ( 1 << 5 ),
    PROJECTILE_VECTOR_CANNOT_BE_ADJUSTED = ( 1 << 6 ),
    PROJECTILES_HAVE_IDENTICAL_ERROR = ( 1 << 7 ),
    PROJECTILES_FIRE_PARALLEL = ( 1 << 8 ),
    CANT_FIRE_WHEN_OTHERS_FIRING = ( 1 << 9 ),
    CANT_FIRE_WHEN_OTHERS_RECOVERING = ( 1 << 10 ),
    DONT_CLEAR_FIRE_BIT_AFTER_RECOVERING = ( 1 << 11 ),
    STAGGER_FIRE_ACROSS_MULTIPLE_MARKS = ( 1 << 12 ),
    FIRES_LOCKED_PROJECTILES = ( 1 << 13 ),
};

enum class PredictionType_t : int16_t
{
    NONE,
    CONTINUOUS,
    INSTANT,
    // Not sure if these are right.. The DMR seems to be #4
};

enum class FiringNoise_t : int16_t
{
    SILENT,
    MEDIUM,
    LOUD,
    VERYLOUD,
    QUIET,
};

enum class DistributionFunction_t : int16_t
{
    POINT,
    HORIZONTAL_FAN
};

enum class DamageType_t : int8_t
{
    GUARDIANS_UNKNOWN,
    GUARDIANS,
    GUARDIANS_SCRIPTING,
    SUICIDE,
    MAGNUM,
    ASSAULT_RIFLE,
    DMR,
    SHOTGUN,
    SNIPER,
    ROCKET_LAUNCHER,
    SPARTAN_LASER,
    FRAG_GRENADE,
    GRENADE_LAUNCHER,
    PLASMA_PISTOL,
    NEEDLER,
    PLASMA_RIFLE,
    PLASMA_REPEATER,
    NEEDLE_RIFLE,
    SPIKER,
    PLASMA_LAUNCHER,
    GRAVITY_HAMMER,
    ENERGY_SWORD,
    PLASMA_GRENADE,
    CONCUSSION_RIFLE,
    GHOST,
    REVENANT,
    REVENANT_GUNNER,
    WRAITH,
    WRAITH_TURRET,
    BANSHEE,
    BANSHEE_BOMB,
    SERAPH,
    MONGOOSE,
    WARTHOG,
    WARTHOG_CHAINGUN,
    WARTHOG_GAUSS,
    WARTHOG_ROCKET,
    SCORPION,
    SCORPION_TURRET,
    FALCON,
    FALCON_GUNNER,
    FALLING,
    COLLISION,
    MELEE,
    EXPLOSION,
    BIRTHDAY_EXPLOSION,
    FLAG,
    BOMB,
    BOMB_EXPLOSION,
    BALL,
    TELEPORTER,
    TRANSFER_DAMAGE,
    ARMOR_LOCK,
    TARGET_LOCATOR,
    HUMAN_TURRET,
    PLASMA_CANNON,
    PLASMA_MORTAR,
    PLASMA_TURRET,
    SHADE_TURRET,
    SABRE,
    SMG,
    CARBINE,
    BATTLE_RIFLE,
    FOCUS_RIFLE,
    FUEL_ROD,
    MISSILE_POD,
    BRUTE_SHOT,
    FLAMETHROWER,
    SENTINEL_GUN,
    SPIKE_GRENADE,
    FIREBOMB_GRENADE,
    ELEPHANT_TURRET,
    SPECTRE,
    SPECTRE_GUNNER,
    TANK,
    CHOPPER,
    FALCON2,
    MANTIS,
    PROWLER,
    SENTINEL_BEAM,
    SENTINEL_RPG,
    TRIPMINE,
};

struct FirstPersonOffsetsSubTag
{
    float x;
    float y;
    float z;
};

class BarrelsSubTag
{
public:
    BarrelFlags_t flags; //0x0000
    float roundsPerSecondMin; //0x0004
    float roundsPerSecondMax; //0x0008
    uint32_t accelTimeFunction_StringID; //0x000C
    float accelTime; //0x0010
    uint32_t decelTimeFunction_StringID; //0x0014
    float decelTime; //0x0018
    float barrelSpinScale; //0x001C
    float blurredRateOfFire; //0x0020
    int16_t shotsPerFireMin; //0x0024
    int16_t shotsPerFireMax; //0x0026
    float fireRecoveryTime; //0x0028
    float softRecoveryFraction; //0x002C
    char pad_0030[8]; //0x0030
    int16_t magazine; //0x0038
    int16_t roundsPerShot; //0x003A
    int16_t minRoundsLoaded; //0x003C
    int16_t roundsBetweenTracers; //0x003E
    uint32_t optionalBarrelMarkerName_StringID; //0x0040
    PredictionType_t predictionType; //0x0044
    FiringNoise_t firingNoise; //0x0046
    float N000018C7; //0x0048
    float accelerationTime; //0x004C
    float decelerationTime; //0x0050
    float damageErrorMin; //0x0054
    float damageErrorMax; //0x0058
    float baseTurningSpeed; //0x005C
    float dynamicTurningSpeedMin; //0x0060
    float dynamicTurningSpeedMax; //0x0064
    DistributionFunction_t distributionFunction; //0x0068
    int16_t projectilesPerShot; //0x006A
    float distributionAngle; //0x006C
    float minimumError; //0x0070
    float errorAngleMin; //0x0074
    float errorAngleMax; //0x0078
    float reloadPenalty; //0x007C
    float switchPenalty; //0x0080
    float zoomPenalty; //0x0084
    float jumpPenalty; //0x0088
    SubTagListRef<UnknownTag> firingPenaltyFunction; //0x008C
    SubTagListRef<UnknownTag> firingCrouchedPenaltyFunction; //0x0098
    SubTagListRef<UnknownTag> movingPenaltyFunction; //0x00A4
    SubTagListRef<UnknownTag> turningPenaltyFunction; //0x00B0
    float errorAngleMaxRotation; //0x00BC
    SubTagListRef<UnknownTag> dualFiringPenaltyFunction; //0x00C0
    SubTagListRef<UnknownTag> dualFiringCrouchedPenaltyFunction; //0x00CC
    SubTagListRef<UnknownTag> dualMovingPenaltyFunction; //0x00D8
    SubTagListRef<UnknownTag> dualTurningPenaltyFunction; //0x00E4
    float dualErrorAngleMaxRotation; //0x00F0
    SubTagListRef<FirstPersonOffsetsSubTag> firstPersonOffsets; //0x00F4
    DamageType_t damageReportingType; //0x0100
    char pad_0101[3]; //0x0101
    TagRef initialProjectile; //0x0104
    TagRef trailingProjectile; //0x0114
    TagRef damageEffect; //0x0124
    TagRef crateProjectile; //0x0134
    float crateProjectileSpeed; //0x0144
    float ejectionPortRecoveryTime; //0x0148
    float illuminationRecoveryTime; //0x014C
    float heatGeneratedPerRound; //0x0150
    uint32_t heatPerRoundFunction_StringID; //0x0154
    float ageGeneratedPerRoundMP; //0x0158
    float ageGeneratedPerRoundSP; //0x015C
    float overloadTime; //0x0160
    float firingEffectDecelTime; //0x0164
    float N00001967; //0x0168
    float rateOfFireAccelTime; //0x016C
    float rateOfFireDecelTime; //0x0170
    float bloomRateOfDecay; //0x0174
    SubTagListRef<UnknownTag> firingEffects; //0x0178
}; //Size: 0x0184

class weap
{
public:
    int16_t objType; //0x0000
    char pad_0002[2]; //0x0002
    GeneralFlags_t flags; //0x0004
    float boundingRadius; //0x0008
    float boundingOffsetX; //0x000C
    float boundingOffsetY; //0x0010
    float boundingOffsetZ; //0x0014
    float horizontalAccelScale; //0x0018
    float verticalAccelScale; //0x001C
    float angularAccelScale; //0x0020
    LightMapSize_t lightMapShadowModeSize; //0x0024
    SweetenerSize_t sweetenerSize; //0x0026
    WaterDensity_t waterDensity; //0x0027
    char pad_0028[4]; //0x0028
    float dynamicLightSphereRadius; //0x002C
    float dynamicLightSphereXOffset; //0x0030
    float dynamicLightSphereYOffset; //0x0034
    float dynamicLightSphereZOffset; //0x0038
    uint32_t genericHudText_StringID; //0x003C
    TagRef genericNameList; //0x0040
    TagRef genericServiceTagList; //0x0050
    uint32_t defaultVariant_StringID; //0x0060
    TagRef model; //0x0064
    TagRef crateObject; //0x0074
    TagRef collisionDamage; //0x0084
    TagRef brittleCollisionDamage; //0x0094
    SubTagListRef<UnknownTag> earlyMoverProperties; //0x00A4
    TagRef creationEffect; //0x00B0
    TagRef materialEffects; //0x00C0
    TagRef meleeImpact; //0x00D0
    SubTagListRef<UnknownTag> AI_Properties; //0x00E0
    SubTagListRef<UnknownTag> functions; //0x00EC
    SubTagListRef<UnknownTag> N000017D2; //0x00F8
    int16_t HUDtextMessageIndex; //0x0104
    char pad_0106[2]; //0x0106
    SubTagListRef<UnknownTag> attachments; //0x0108
    SubTagListRef<UnknownTag> materialResponses; //0x0114
    SubTagListRef<UnknownTag> jetWash; //0x0120
    SubTagListRef<UnknownTag> widgets; //0x012C
    SubTagListRef<UnknownTag> changeColor; //0x0138
    SubTagListRef<UnknownTag> predictedREsources; //0x0144
    SubTagListRef<UnknownTag> multiplayerObjectProperties; //0x0150
    TagRef simulationInterpolation; //0x015C
    SubTagListRef<UnknownTag> unkTagList; //0x016C
    uint32_t itemFlags; //0x0178
    int16_t oldMessageIndex; //0x017C
    int16_t sortOrder; //0x017E
    float oldMPonGroundScale; //0x0180
    float oldSPonGroundScale; //0x0184
    uint32_t pickupMessage_StringID; //0x0188
    uint32_t swapMessage_StringID; //0x018C
    uint32_t pickupOrDualWieldMessage_StringID; //0x0190
    uint32_t swapOrDualWieldMessage_StringID; //0x0194
    uint32_t pickedUpMessage_StringID; //0x0198
    uint32_t switchToMessage_StringID; //0x019C
    uint32_t switchToFromAIMessage_StringID; //0x01A0
    uint32_t allWeaponsEmptyMessage_StringID; //0x01A4
    uint32_t overheatedMessage_StringID; //0x01A8
    TagRef collisionSound; //0x01AC
    char pad_01BC[12]; //0x01BC
    TagRef detonationDamageEffect; //0x01C8
    float detonationDelayMin; //0x01D8
    float detonationDelayMax; //0x01DC
    TagRef detonatingEffect; //0x01E0
    TagRef detonationEffect; //0x01F0
    float globalGroundScale; //0x0200
    float smallHoldScale; //0x0204
    float smallHolsterScale; //0x0208
    float largeHoldScale; //0x020C
    float largeHolsterScale; //0x0210
    float mediumHoldScale; //0x0214
    float mediumHolsterScale; //0x0218
    float hugeHoldScale; //0x021C
    float hugeHolsterScale; //0x0220
    TagRef groundedFriction; //0x0224
    uint32_t weaponFlags; //0x0234
    uint32_t weaponFlags2; //0x0238
    uint32_t unk_StringID; //0x023C
    int16_t secondaryTriggerMode; //0x0240
    int16_t maximumAltFireShotsLoaded; //0x0242
    float turnOnTime; //0x0244
    float readyTime; //0x0248
    TagRef readyEffect; //0x024C
    TagRef readyDamageEffect; //0x025C
    float heatRecoveryThreshold; //0x026C
    float overheatedThreshold; //0x0270
    float heatDetonationThreshold; //0x0274
    float heatDetonationFraction; //0x0278
    float heatLossPerSecond; //0x027C
    uint32_t heatLossFunction_StringID; //0x0280
    uint32_t customFunction1_StringID; //0x0284
    float customFunctionValue; //0x0288
    char pad_028C[4]; //0x028C
    float N0000181E; //0x0290
    float heatIllumination; //0x0294
    float overheated_heatLossPerSecond; //0x0298
    uint32_t customFunction2_StringID; //0x029C
    TagRef overheated; //0x02A0
    TagRef overheatDamageEffect; //0x02B0
    TagRef detonation; //0x02C0
    TagRef detonationDamageEffect2; //0x02D0
    SubTagListRef<UnknownTag> meleeDamage; //0x02E0
    TagRef clashEffect; //0x02EC
    int8_t meleeDamageReportType; //0x02FC
    char pad_02FD[1]; //0x02FD
    int16_t zoomLevels; //0x02FE
    float zoomPowerMin; //0x0300
    float zoomPowerMax; //0x0304
    float autoaimAngle; //0x0308
    float autoaimRangeLong; //0x030C
    float autoaimRangeShort; //0x0310
    float autoaimSafeRadius; //0x0314
    float magnetismAngle; //0x0318
    float magnetismRangeLong; //0x031C
    float magnetismRangeShort; //0x0320
    float magnetismSafeRadius; //0x0324
    float deviationAngle; //0x0328
    char pad_032C[24]; //0x032C
    SubTagListRef<UnknownTag> targetTracking; //0x0344
    char pad_0350[16]; //0x0350
    int16_t movementPenalized; //0x0360
    char pad_0362[2]; //0x0362
    float forwardMovePenalty; //0x0364
    float sideMovePenalty; //0x0368
    float AI_scariness; //0x036C
    float weaponPowerOnTime; //0x0370
    float weaponPowerOffTime; //0x0374
    TagRef weaponPowerOnEffect; //0x0378
    TagRef weaponPowerOffEffect; //0x0388
    float ageHeatRecoveryPenalty; //0x0398
    float ageRateOfFirePenalty; //0x039C
    float ageMisfireStart; //0x03A0
    float ageMisfireChance; //0x03A4
    TagRef pickupSound; //0x03A8
    TagRef zoomInSound; //0x03B8
    TagRef zoomOutSound; //0x03C8
    float activeCamoDing; //0x03D8
    uint32_t handleNode_StringID; //0x03DC
    uint32_t weaponClass_StringID; //0x03E0
    uint32_t weaponName_StringID; //0x03E4
    int16_t weaponType; //0x03E8
    char pad_03EA[18]; //0x03EA
    SubTagListRef<UnknownTag> firstPersonModelANDAnim; //0x03FC
    TagRef HUDinterface; //0x0408
    TagRef N0000184E; //0x0418
    char pad_0428[12]; //0x0428
    SubTagListRef<MagazineSubTag> magazines; //0x0434
    SubTagListRef<UnknownTag> newTriggers; //0x0440
    SubTagListRef<BarrelsSubTag> barrels; //0x044C
    float N00001856; //0x0458
    float N00001880; //0x045C
    float maxMoveAccel; //0x0460
    float maxMoveVelo; //0x0464
    float maxTurningAccel; //0x0468
    float maxTurningVelo; //0x046C
    TagRef deployedVehicle; //0x0470
    TagRef deployedWeapon; //0x0480
    TagRef ageModel; //0x0490
    TagRef ageWeapon; //0x04A0
    TagRef agedMaterialEffects; //0x04B0
    float hammerAgePerUse_MP; //0x04C0
    float hammerAgePerUse_SP; //0x04C4
    char pad_04C8[4]; //0x04C8
    float viewModelX; //0x04CC
    float viewModelY; //0x04D0
    float viewModelZ; //0x04D4
    float scopeSizeX; //0x04D8
    float scopeSizeY; //0x04DC
    float thirdPersonPitchBoundMin; //0x04E0
    float thirdPersonPitchBoundMax; //0x04E4
    float zoomTransitionTime; //0x04E8
    float meleeWeaponDelay; //0x04EC
    int32_t weaponHolsterMarker_StringID; //0x04F0
    SubTagListRef<UnknownTag> weaponScreenEffect; //0x04F4
}; //Size: 0x0500