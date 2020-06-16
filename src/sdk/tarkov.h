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
    char pad58[0x58];
    class ArrayItems* m_pItemList; //0x0058
    char pad_18[0x18]; //0x0060
    class Array* m_pPlayerList; //0x0078
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

class HandsController
{
public:
    char pad_d0[0xD0];
    void *fireport;
    char extra[0x100];
};
class Player
{
public:
    char pad_0000[24]; //0x0000
    class LocalPlayerChecker *m_pLocalPlayerChecker; //0x0018
    char pad_0020[24]; //0x0020
    class MovementContext *m_pMovementContext; //0x0038
    char pad_60[0x60];
    class PlayerBody *m_pPlayerBody; //0x00A0
    char pad_c0[0xC0];
    class ProceduralWeaponAnimation *m_pProceduralWeaponAnimation; //0x0168
    char pad_270[0x270];
    class PlayerProfile *m_pPlayerProfile; //0x03E0
    char pad_028[0x28];
    class HealthController *m_pHealthController; //0x0410
    char _paderino[16];
    class HandsController *m_pHandsController; // 0x0428
};

class MovementContext
{
public:
    char pad_0000[480]; //0x0000
    Vector2D ViewAngles; //0x01E0
    Vector2D ViewAngles2; //0x01E8
    Vector3D LocalPosition; //0x01F0
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
    char pad_0000[88]; //0x0000
    Matrix4x4 ViewMatrix0; //0x0058
    char pad_0098[64]; //0x0098
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
}; //Size: 0x0050

class PlayerInfo
{
public:
    char pad_0000[16]; //0x0000
    class UnityEngineString* m_pPlayerName; //0x0010
    char pad_0018[56]; //0x0018
    int32_t Side; //0x0050
    int32_t CreationDate; //0x0054
}; //Size: 0x0058

class UnityEngineString
{
public:
    char pad_0000[16]; //0x0000
    int32_t size; //0x0010
    //wchar_t name[10]; //0x0014
    char16_t name[10]; //0x0014
}; //Size: 0x0028

class HealthController // _healthController
{
public:
    char pad_0000[24]; //0x0000
    class HealthBody* m_pHealthBody; //0x0018
}; //Size: 0x0020

class HealthBody
{
public:
    char pad_0000[24]; //0x0000
    class BodyController* m_pBodyController; //0x0018
}; //Size: 0x0020

class BodyController
{
public:
    char pad_0000[48]; //0x0000
    class HealthContainer* m_pHealthHead; //0x0030
    char pad_0038[16]; //0x0038
    class HealthContainer* m_pHealthThorax; //0x0048
    char pad_0050[16]; //0x0050
    class HealthContainer* m_pHealthStomach; //0x0060
    char pad_0068[16]; //0x0068
    class HealthContainer* m_pHealthLeftArm; //0x0078
    char pad_0080[16]; //0x0080
    class HealthContainer* m_pHealthRightArm; //0x0090
    char pad_0098[16]; //0x0098
    class HealthContainer* m_pHealthLeftLeg; //0x00A8
    char pad_00B0[16]; //0x00B0
    class HealthContainer* m_pHealthRightLeg; //0x00C0
}; //Size: 0x00C8

class HealthContainer
{
public:
    char pad_0000[16]; //0x0000
    class Health* m_pHealth; //0x0010
}; //Size: 0x0018

class Health
{
public:
    char pad_0000[16]; //0x0000
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

class ArrayItems
{
public:
    char pad_0000[16]; //0x0000
    class ItemList* m_pItemList; //0x0010
    int32_t Count; //0x0018
    int32_t MaxCount; //0x001C
}; //Size: 0x0020

class ItemList
{
public:
    char pad_0000[32]; //0x0000
    class Item* m_pFirstItem; //0x0020
    class Item* m_pSecondItem; //0x0028
}; //Size: 0x0030

class Item
{
public:
    char pad_0000[16]; //0x0000
    class ItemProfile* m_pItemProfile; //0x0010
}; //Size: 0x0018

class ItemProfile
{
public:
    char pad_0000[40]; //0x0000
    class ItemStats* m_pItemStats; //0x0028
    class ItemBasicInformation* m_pItemInformation; //0x0030
}; //Size: 0x0038

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
    char pad_0000[56]; //0x0000
    class ItemLocationContainer* m_pItemLocationContainer; //0x0038
}; //Size: 0x0040

class ItemLocationContainer
{
public:
    char pad_0000[176]; //0x0000
    Vector3D ItemPosition; //0x00B0
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

class ProceduralWeaponAnimation
{
public:
    char pad_0000[40]; //0x0000
    class BreathEffector* m_pBreath; //0x0028
    char pad_0030[24]; //0x0030
    class ShotEffector* m_pShooting; //0x0048
    char pad_0050[56]; //0x0050
    class OpticSight* m_pOpticSightArray; //0x0088
    char pad_0090[80]; //0x0090
    int32_t Mask; //0x00E0 Write zero for no-recoil / no - sway
    char pad_00E4[44]; //0x00E4
    float CameraSmoothTime; //0x0110
    char pad_0114[192]; //0x0114
    float CameraSmoothSteady; //0x01D4
}; //Size: 0x01D8

class ItemStats
{
public:
    char pad_0000[80]; //0x0000
    class GameItem* m_pGameItem; //0x0050
}; //Size: 0x0058

class StatsNames
{
public:
    char pad_0000[32]; //0x0000
    class ItemNames* m_pItemNames; //0x0020
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
    char pad_0000[104]; //0x0000
    float Intensity; //0x0068
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

class ItemTemplate
{
public:
    char pad_0000[16]; //0x0000
    class UnityEngineString* m_pName; //0x0010
    char pad_0018[16]; //0x0018
    class UnityEngineString* m_ItemSoundName; //0x0028
    char pad_0030[24]; //0x0030
    class UnityEngineString* m_pSlotName; //0x0048
    class UnityEngineString* m_pHashName; //0x0050
    char pad_0058[8]; //0x0058
    class UnityEngineString* m_pHashCategory; //0x0060
}; //Size: 0x0068