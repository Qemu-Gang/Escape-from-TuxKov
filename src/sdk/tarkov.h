#pragma once

#include "Vector.h"

#include <cstdint>

class LOCALGAMEWORLD // Aka LocalGameWorld
{
public:
    /*
    char pad_0000[24]; //0x0000
    class ExitController* m_pExitController; //0x0018
    char pad_0020[24]; //0x0020
    class UnityEngineString* m_LocalPlayerID; //0x0038
    char pad_0038[24]; //0x0040
     */
    char pad58[0x68];
    class LootArray* m_pItemList; //0x0068
    char pad_18[0x18]; //0x0070
    class Array* m_pPlayerList; //0x0088
}; //Size: 0x0078

class Array
{
public:
    char pad_0000[0x10]; //0x0000
    class List* m_pList; //0x0010
    int32_t Count; //0x0018
    int32_t MaxCount; //0x001C
}; //Size: 0x0020

class List
{
public:
    char pad_0000[0x20]; //0x0000
    class Player* m_pFirstEntity; //0x0020
    class Player* m_pSecondPlayer; //0x0028
}; //Size: 0x0030

class WeaponTemplate
{
public:
    char _pad[0x24C];
    bool m_allowJam;
    bool m_allowFeed;
    bool m_allowMisfire;
    bool m_allowSlide;
};
class WeaponItem
{
public:
    char _pad[0x10];
    class UnityEngineString* m_Id;      // 0x10
    class ItemAddress* m_itemAddress;   // 0x18
    class Components* m_components;     // 0x20
    class StringCache* m_stringCache;   // 0x28
    class ItemAddress* m_currentAddress;// 0x30
    class asdfasdf* m_ChildrenChanged;  // 0x38
    class WeaponTemplate* m_BackingField; // 0x40 //EFT.InventoryLogic.WeaponTemplate
    class List*         m_Attributes;   // 0x48
    class Nullable*     m_BackingField2;// 0x50
    uint32_t m_bUnlimitedCount;         // 0x58
    int32_t m_BuyRestrictionMax;        // 0x5C
    int32_t m_BuyRestrictionCurrent;    // 0x60
    int32_t m_StackObjectsCount;        // 0x64
    int32_t m_Version;                  // 0x68
    int32_t m_SpawnedInSession;         // 0x6C
    class Grid* m_Grids;                // 0x70
    class Slot* m_Slots;                // 0x78
    class RepairableComponent* m_Repairable; // 0x80
    class FoldableComponent* m_Foldable;// 0x88
    class FireModeComponent* m_FireMode;// 0x90
    class Slot* m_Chambers;             // 0x98 [pointer to Array]
    class Vector3D* m_OpticCalibrationPts;// 0xA0
    class Vector3D* m_OpticTrajInfos;   // 0xA8
    class asdfff* m_MalfState;          // 0xB0
    class Slot* m_magSlotCache;         // 0xB8
    // aimindex
    //onmalfunctionvalidate
    //shellsinchambers
    //compatibleammo
    //armed
    //cylinderhammerclosed
};
class HandsController
{
public:
    char pad_60[0x60];
    class WeaponItem* m_pWeaponItem; // 0x60       EFT.InventoryLogic.Item
};
class Player
{
public:
    char pad_0000[24]; //0x0000
    class LocalPlayerChecker *m_pLocalPlayerChecker; //0x0018
    char pad_0020[32]; //0x0020
    class MovementContext *m_pMovementContext; //0x0040
    char pad_60[96];
    class PlayerBody *m_pPlayerBody; //0x00A8
    char pad_c0[232];
    class ProceduralWeaponAnimation *m_pProceduralWeaponAnimation; //0x0198
    char pad_270[672];
    char pad_a0[0xB0];
    class PlayerProfile *m_pPlayerProfile; //0x04F0
    char pad_028[0x30];
    class HealthController *m_pHealthController; //0x0528
    char _paderino[0x18];
    class HandsController *m_pHandsController; // 0x0540
};

class MovementContext
{
public:
    char pad_0000[0x22C]; //0x0000
    Vector2D ViewAngles;
    Vector2D ViewAngles2;
    Vector3D LocalPosition;
}; //Size: 0x01FC

class CAMERA // Aka FPS Camera
{
public:
    char pad_0000[48]; //0x0000
    class FPSCamera* m_pFpsCamera; //0x0030
}; //Size: 0x0038

class FPSCamera
{
public:
    char pad_0000[48]; //0x0000
    class CameraObject* m_pCameraObject; //0x0030
    char pad_0038[40]; //0x0038
    char* ObjectName; //0x0060
    char pad_0068[8]; //0x0068
    class OpticCameraManager* m_pOpticCameraManager; // 0x0070
}; //Size: 0x0068

class CameraObject
{
public:
    char pad_0000[8]; //0x0000
    class CameraContainer* m_pCameraContainer; //0x0008
    char pad_0010[8]; //0x0010
    class CameraEntity* m_pCameraEntity; //0x0018
}; //Size: 0x0020

class OpticCameraManager
{
public:
    char pad_0000[16]; //0x0000
    class TemplateCamera* m_pTemplateCamera; // 0x0010 aka ScopeCamera
    char pad_0001[16]; //0x0018
    class OpticSight* m_pOpticSight; // 0x0028
};

class TemplateCamera
{
public:
    char pad_0000[48]; //0x0000
    class CameraObject* m_pCameraObject; //0x0030
};

class CameraEntity
{
public:
    char pad_0000[0xDC];
    Matrix4x4 ViewMatrix; //0x00D8
    char pad_00E0[64]; //0x00E0
    float FOVY; // 0x0098
    char pad_009C[20]; //0x009C
    float FOVX; // 0x00AC
}; //Size: 0x0118

class PlayerBody
{
public:
    char pad_0000[40]; //0x0000
    class m_pSkeletonRootJoin* m_pSkeletonRootJoin; //0x0028
}; //Size: 0x0030

class m_pSkeletonRootJoin
{
public:
    char pad_0000[40]; //0x0000
    class BoneEnumerator* m_pBoneEnumerator; //0x0028
}; //Size: 0x0030

class BoneEnumerator
{
public:
    char pad_0000[16]; //0x0000
    class TransformArray* m_pTransformArray; //0x0010
}; //Size: 0x0018

class TransformArray
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class PlayerProfile
{
public:
    char pad_0000[16]; //0x0000
    class UnityEngineString* m_pPlayerId; //0x0010
    char pad_0018[16]; //0x0018
    class PlayerInfo* m_PlayerInfo; //0x0028
    char pad_0030[16]; //0x0030
    class PlayerHealth* m_pHealth; //0x0040
    class Inventory* m_pInventory; //0x0048
    void* pad;                     //0x0050
    void* pad2;                    //0x0058
    void* skills;                  //0x0060
}; //Size: 0x0050

class PlayerInfoSettings
{
    char pad[0x10];
    int role;          // 0x10
    int botDifficulty; // 0x14
};
class PlayerInfo
{
public:
    char pad_0000[0x10]; //0x0000
    class UnityEngineString* m_pPlayerName; // 0x10
    class UnityEngineString* m_pNickname;   // 0x18
    class UnityEngineString* m_pGroupId;    // 0x20
    class UnityEngineString* m_pEntryPoint; // 0x28
    class UnityEngineString* m_pVoice;      // 0x30
    void *list;
    void *pad;
    class PlayerInfoSettings* settings;     // 0x48
    void *action1;                          // 0x50
    void *action2;                          // 0x58
    void *action3;                          // 0x60
    int32_t Side;                           // 0x68
    int32_t CreationDate;                   // 0x6c
    double savageLockTime;
    int64_t nicknameChangeDate;
    int isStreamerModeAvailable;
    int memberCategory;
}; //Size: 0x0058

class UnityEngineString
{
public:
    char pad_0000[16]; //0x0000
    int32_t size; //0x0010
    //wchar_t name[24]; //0x0014
    char16_t name[16]; //0x0014
}; //Size: 0x0028

class HealthController // _healthController
{
public:
    char pad_0000[0x50]; //0x0000
    class HealthBody* m_pHealthBody;
}; //Size: 0x0020

class HealthBody
{
public:
    char pad_0000[0x18]; //0x0000
    class BodyController* m_pBodyController; //0x0018
}; //Size: 0x0020


enum BodyParts_t
{
    HEAD = 0,
    THORAX,
    STOMACH,
    LEFTARM,
    RIGHTARM,
    LEFTLEG,
    RIGHTLEG,
    NUM_BODY_PARTS
};

class BodyPartContainer
{
public:
    char _pad[0x10];
    class HealthContainer* m_pBodyPart;
};
class BodyController
{
public:
    char pad_0000[0x20]; //0x0000

    BodyPartContainer m_bodyParts[NUM_BODY_PARTS];
}; //Size: 0x00C8

class HealthContainer
{
public:
    char pad_0000[0x10]; //0x0000
    class Health* m_pHealth; //0x0010
}; //Size: 0x0018

class Health
{
public:
    char pad_0000[0x10]; //0x0000
    float Health; //0x0010
    float HealthMax; //0x0014
}; //Size: 0x0018

class PlayerHealth
{
public:
    char pad_0000[24]; //0x0000
    class HealthInfo* m_playerHealthInfo; //0x0018
}; //Size: 0x0020

class HealthInfo
{
public:
    char pad_0000[16]; //0x0000
    float CurentHealth; //0x0010
    float MaxHealth; //0x0014
}; //Size: 0x0018

class CameraContainer
{
public:
    char pad_0000[56]; //0x0000
    class CameraPosition* m_pCameraPosition; //0x0038
}; //Size: 0x0040

class CameraPosition
{
public:
    char pad_0000[176]; //0x0000
    Vector3D cameraPosition; //0x00B0
    char pad_00BC[4]; //0x00BC
    Vector3D cameraRotation; //0x00C0
}; //Size: 0x00D0

class LootArray
{
public:
    char pad_0000[0x10]; //0x0000
    class ItemList* m_pItemList; //0x0010
    int32_t Count; //0x0018
    int32_t MaxCount; //0x001C
}; //Size: 0x0020

class ItemList
{
public:
    char pad_0000[0x20]; //0x0000
    class Item* m_pFirstItem; //0x0020
    class Item* m_pSecondItem; //0x0028
}; //Size: 0x0030

class Item
{
public:
    char pad_0000[0x10]; //0x0000
    class ItemProfile* m_pItemProfile; //0x0010
}; //Size: 0x0018

class ItemProfile
{
public:
    char pad_0000[0x28]; //0x0000
    class ItemStats* m_pItemStats; //0x0028
    class ItemBasicInformation* m_pItemInformation; //0x0030
}; //Size: 0x0038S

class ItemBasicInformation
{
public:
    char pad_0000[48]; //0x0000
    class ItemLocalization* m_pItemLocalization; //0x0030
    char pad_0038[40]; //0x0038
    char* ItemPatName; //0x0060
}; //Size: 0x0068

class ItemLocalization
{
public:
    char pad_0000[8]; //0x0000
    class ItemCoordinates* m_pItemCoordinates; //0x0008
}; //Size: 0x0010

class ItemCoordinates
{
public:
    char pad_0000[0x38]; //0x0000
    class ItemLocationContainer* m_pItemLocationContainer; //0x0038
}; //Size: 0x0040

class ItemLocationContainer
{
public:
    char pad_0000[0x90]; //0x0000
    Vector3D ItemPosition;
}; //Size: 0x00BC

class Inventory
{
public:
    char pad_0000[48]; //0x0000
    class FastAccess* m_pFastAccess; //0x0030
}; //Size: 0x0038

class FastAccess
{
public:
    char pad_0000[16]; //0x0000
    class BoundCells* m_pBoundCells; //0x0010
}; //Size: 0x0018

class BoundCells
{
public:
    char pad_0000[24]; //0x0000
    class CollectionEntry* Entries; //0x0018
    char pad_0020[32]; //0x0020
    int32_t Count; //0x0040
    int32_t Max; //0x0044
}; //Size: 0x0048

class m_pDictionaryItems
{
public:
    char pad_0000[32]; //0x0000
    class Weapon* Knife; //0x0020
    class Weapon* Pistol; //0x0028
    class Weapon* FirstWeapon; //0x0030
    class Weapon* SecondWeapon; //0x0038
    char pad_0040[64]; //0x0040
}; //Size: 0x0080

class Weapon
{
public:
    char pad_0000[32]; //0x0000
    class WeaponDetails* m_pWeaponDetails; //0x0020
}; //Size: 0x0028

class WeaponDetails
{
public:
    char pad_0000[32]; //0x0000
    class WeaponNameInfo* m_pWeaponNameInfo; //0x0020
}; //Size: 0x0028

class WeaponNameInfo
{
public:
    char pad_0000[16]; //0x0000
    class UnityEngineString* Name; //0x0010
    char pad_0018[16]; //0x0018
    class UnityEngineString* NameType; //0x0028
}; //Size: 0x0030

class FirearmController
{
public:
    char _padd[0x60];
    class WeaponItem *m_pItem;
    char _pad[0xD8];
    float m_hipInaccuracy; // 0x140
    float m_weaponLn; // weapon length
};

class ProceduralWeaponAnimation
{
public:
    char pad_0000[0x28]; //0x0000
    class BreathEffector *m_pBreath;         //0x0028
    class WalkEffector *m_pWalk;             // 0x30
    class MotionEffector *m_pMotionReact;    // 0x38
    class ForceEffector *m_pForceReact;      // 0x40
    class ShotEffector *m_pShooting;         // 0x48
    class TurnAwayEffector *m_pTurnaway;     // 0x50
    char pad_0050[0x28];                     //0x0058
    class FirearmController *m_pFiring;      //0x0080
    char pad_0088[0x78];                     //0x0088
    uint32_t mask; //0x0100
    float sway1;
    float sway2;
    float sway3;
}; //Size: 0x01D8

class ItemStats
{
public:
    char pad_0000[0xB0]; //0x0000
    class GameItem* m_pGameItem;
}; //Size: 0x0058

class StatsNames
{
public:
    char pad_0000[0x40]; //0x0000
    class ItemNames* m_pItemNames; //0x0040
}; //Size: 0x0028

class ItemNames
{
public:
    char pad_0000[80]; //0x0000
    class UnityEngineString* m_pItemHashIdentifier; //0x0050
    char pad_0058[8]; //0x0058
    class UnityEngineString* m_pParentHashIdentifier; //0x0060
}; //Size: 0x0068

class OpticSight
{
public:
    char pad_0000[40]; //0x0000
    class UnityCamera* m_pCamera; //0x0020
}; //Size: 0x0028

class UnityCamera
{
public:
    char pad_0000[40]; //0x0000
    class CameraRaw* m_pCameraRaw; //0x0028
}; //Size: 0x0030

class BreathEffector
{
public:
    char pad_0000[0xA0]; //0x0000
    bool IsAiming; //0x00A0
    char pad_0091[3];
    float Intensity; //0x00A4
}; //Size: 0x0098

class ShotEffector
{
public:
    char pad_0000[0x6C]; //0x0000
    float padyolo;
    float recoilHori;
    float recoilVert;
    float Intensity4;
    float Intensity5;
    float Intensity6;
    float Intensity7;
}; //Size: 0x006C

class CameraRaw
{
public:
    char pad_0000[48]; //0x0000
    class CameraEntity* m_pCameraEntity; //0x0030
}; //Size: 0x0038

class PlayerId
{
public:
    char pad_0000[16]; //0x0000
    int32_t N000007E4; //0x0010
    wchar_t N000007F5[24]; //0x0014
    char pad_0044[112]; //0x0044
}; //Size: 0x00B4

class LocalPlayerChecker
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class ExitController
{
public:
    char pad_0000[32]; //0x0000
    class ExitPoint* m_pExitPoint; //0x0020
}; //Size: 0x0028

class ExitPoint
{
public:
    char pad_0000[24]; //0x0000
    int32_t ExitCount; //0x0018
    char pad_001C[4]; //0x001C
    class Extraction* m_pFirstEntry; //0x0020
    class Extraction* m_pSecondEntry; //0x0028
}; //Size: 0x0030

class Extraction
{
public:
    char pad_0000[16]; //0x0000
    class PositionStuff* m_pPositionStuff; //0x0010
    char pad_0018[64]; //0x0018
    class ExtractionName* m_pExtractName; //0x0058
}; //Size: 0x0060

class ExtractionName
{
public:
    char pad_0000[16]; //0x0000
    class UnityEngineString* Name; //0x0010
}; //Size: 0x0018

class PositionStuff
{
public:
    char pad_0000[48]; //0x0000
    class PositionStuff2* m_pPositionStuff2; //0x0030
}; //Size: 0x0038

class PositionStuff2
{
public:
    char pad_0000[48]; //0x0000
    class PositionStuff3* m_pPositionStuff3; //0x0030
}; //Size: 0x0038

class PositionStuff3
{
public:
    char pad_0000[8]; //0x0000
    class ExtractionTransforms* m_pExtractionTransform; //0x0008
}; //Size: 0x0010

class ExtractionTransforms
{
public:
    char pad_0000[40]; //0x0000
    class ExtranctionTransformPointer* m_pExtractionTransform; //0x0028
}; //Size: 0x0030

class ExtranctionTransformPointer
{
public:
    char pad_0000[16]; //0x0000
    class Transform* m_pTransform; //0x0010
}; //Size: 0x0018

class Transform
{
public:
    char pad_0000[40]; //0x0000
    Matrix4x4 ExitMatrix; //0x0028
}; //Size: 0x0068

class CollectionEntry
{
public:
    char pad_0000[48]; //0x0000
    class InventoryLogicSlot* m_pKnife; //0x0030
    char pad_0038[16]; //0x0038
    class InventoryLogicSlot* m_pPistol; //0x0048
    char pad_0050[16]; //0x0050
    class InventoryLogicSlot* m_pFirstWeapon; //0x0060
    char pad_0068[16]; //0x0068
    class InventoryLogicSlot* m_pSecondWeapon; //0x0078
}; //Size: 0x0080

class InventoryLogicSlot
{
public:
    char pad_0000[24]; //0x0000
    class DizBindingBindableState* m_pReactiveContainedItem; //0x0018
}; //Size: 0x0020

class DizBindingBindableState
{
public:
    char pad_0000[16]; //0x0000
    class GameItem* m_pGameItem; //0x0010
}; //Size: 0x0018

class GameItem
{
public:
    char pad_0000[32]; //0x0000
    class ItemTemplate* m_pItemTemplate; //0x0020
}; //Size: 0x0028