#include "esp.h"
#include "esp_loot.h"

#include "../globals.h"
#include "../sdk/unity.h"
#include "peeper/client/peeper.h"
#include "../utils/Logger.h"

#include <csignal>
#include <ios>
#include <iostream>

const int arm_right[] = { Bones::HumanRUpperarm, Bones::HumanRForearm1, Bones::HumanRPalm };
const int arm_left[] = { Bones::HumanLUpperarm, Bones::HumanLForearm1, Bones::HumanLPalm };

const int leg_right[] = { Bones::HumanRThigh1, Bones::HumanRCalf, Bones::HumanRFoot };
const int leg_left[] = { Bones::HumanLThigh1, Bones::HumanLCalf, Bones::HumanLFoot };

void ESP::DrawPlayers( ) {
    LOCALGAMEWORLD gameworld = process->Read<LOCALGAMEWORLD>( gameWorldAddr );

    Array playerList = process->Read<Array>( (uintptr_t)gameworld.m_pPlayerList );

    for( int i = 0; i < playerList.Count; i++ ){
        Vector2D head2D, neck2D, pelvis2D;
        float distance;
        char buffer[8];
        std::string text;
        Color color;
        Player player;
        PlayerProfile profile;
        PlayerInfo info;
        UnityEngineString name;
        HealthController healthController;
        HealthBody healthBody;
        BodyController bodyController;
        float hp = 0;

        uintptr_t playerAddr = process->Read<uintptr_t>((uintptr_t)playerList.m_pList + (0x20 + (i * 8)) );

        if( !playerAddr )
            continue;

        player = process->Read<Player>( playerAddr );

        // Check for localplayer while we're itering over these. Not ideal but whatever
        if( player.m_pLocalPlayerChecker ){
            localPlayerAddr = playerAddr;
            localPlayerHead = Unity::GetBonePosition( localPlayerAddr, Bones::HumanHead );
            continue;
        }

        Vector3D headPos = Unity::GetBonePosition( playerAddr, Bones::HumanHead );
        if( !Unity::World2Screen( headPos, &head2D ) )
            continue;
        Vector3D neckPos = Unity::GetBonePosition( playerAddr, Bones::HumanNeck );
        if( !Unity::World2Screen( neckPos, &neck2D ) )
            continue;
        Vector3D pelvisPos = Unity::GetBonePosition( playerAddr, Bones::HumanPelvis );
        if( !Unity::World2Screen( pelvisPos, &pelvis2D ) )
            continue;

        // Needed bones for anything
        if( !Unity::World2Screen( headPos, &head2D )
        || !Unity::World2Screen( neckPos, &neck2D )
        || !Unity::World2Screen( pelvisPos, &pelvis2D ) ){
            continue;
        }

        // Scav color check
        profile = process->Read<PlayerProfile>( (uintptr_t)player.m_pPlayerProfile );
        info = process->Read<PlayerInfo>( (uintptr_t)profile.m_PlayerInfo );
        name = process->Read<UnityEngineString>( (uintptr_t)info.m_pPlayerName );
        healthController = process->Read<HealthController>( (uintptr_t)player.m_pHealthController );
        healthBody = process->Read<HealthBody>( (uintptr_t)healthController.m_pHealthBody );
        bodyController = process->Read<BodyController>( (uintptr_t)healthBody.m_pBodyController );

        // Calculate HP
        for( int limb = 0; limb < NUM_BODY_PARTS; limb++ )
        {
            HealthContainer bodypart = process->Read<HealthContainer>( (uintptr_t)bodyController.m_bodyParts[limb].m_pBodyPart );
            Health bodypartHealth = process->Read<Health>( (uintptr_t)bodypart.m_pHealth );
            hp += bodypartHealth.Health;
        }

        // humans have creationdate
        if( info.CreationDate > 0 ){
            // player scav
            if( info.Side == 4 )
            {
                color = Color( 28, 67, 193, 255 );
            }
            else
            {
                color = Color( 225, 0, 0, 255 );
            }
        } else {
            color = Color( 255, 179, 71, 255 );
        }

        // Draw Spine
        Peeper::AddLine( head2D.x, head2D.y, neck2D.x, neck2D.y, color, 1.0f );
        Peeper::AddLine( neck2D.x, neck2D.y, pelvis2D.x, pelvis2D.y, color, 1.0f );

        // Draw distance
        if( localPlayerAddr ){
            distance = localPlayerHead.Distance( headPos );
            sprintf(buffer, "%0.1fm", distance );
            text += '(';
            text += buffer;
            text += ')';
            // Draw Name if not scav
            if( info.Side != 4 )
            {
                char playername[64] = { 0 };
                int j = 0;
                // the legendary unicode byte skipper
                for(int byte = 0; (byte < (name.size * 2) + 2); byte++)
                {
                    if((byte % 2) != 0)
                        continue;

                    char ascii = ((char*)(&name.name[0]))[byte];
                    // set this manually like a man
                    if (ascii <= 0 || ascii > 127)
                        continue;

                    playername[j] = ascii;
                    j++;
                }

                playername[std::min(63, (name.size * 2) + 2)] = '\0';
                playername[63] = '\0';
                text += '-';
                text += playername;
            }

            sprintf(buffer, "%0.0f", hp );
            text += " [";
            text += buffer;
            text += ']';

            Peeper::AddText( head2D.x, head2D.y, color, text.c_str() );

            // if they are very far, end here and dont draw more limbs
            if( distance > 200.0f ){
                continue;
            }
        }



        // Draw right arm
        Vector2D previous = neck2D;
        for( const int bone : arm_right ){
            Vector2D bone2D;
            Vector3D bone3D = Unity::GetBonePosition( playerAddr, bone );

            if( !Unity::World2Screen( bone3D, &bone2D ) )
                continue;

            Peeper::AddLine( previous.x, previous.y, bone2D.x, bone2D.y, color, 1.0f );
            previous = bone2D;
        }

        // Draw left arm
        previous = neck2D;
        for( const int bone : arm_left ){
            Vector2D bone2D;
            Vector3D bone3D = Unity::GetBonePosition( playerAddr, bone );

            if( !Unity::World2Screen( bone3D, &bone2D ) )
                continue;

            Peeper::AddLine( previous.x, previous.y, bone2D.x, bone2D.y, color, 1.0f );
            previous = bone2D;
        }

        // Draw right leg
        previous = pelvis2D;
        for( const int bone : leg_right ){
            Vector2D bone2D;
            Vector3D bone3D = Unity::GetBonePosition( playerAddr, bone );

            if( !Unity::World2Screen( bone3D, &bone2D ) )
                continue;

            Peeper::AddLine( previous.x, previous.y, bone2D.x, bone2D.y, color, 1.0f );
            previous = bone2D;
        }

        // Draw left leg
        previous = pelvis2D;
        for( const int bone : leg_left ){
            Vector2D bone2D;
            Vector3D bone3D = Unity::GetBonePosition( playerAddr, bone );

            if( !Unity::World2Screen( bone3D, &bone2D ) )
                continue;

            if( !Peeper::AddLine( previous.x, previous.y, bone2D.x, bone2D.y, color, 1.0f ) ){
                Logger::Log("Warning! Too many draw requests!\n");
            }
            previous = bone2D;
        }
    }

    Peeper::SubmitDraws();
}

void ESP::DrawItems() {
    char nameBuffer[128];
    float distance;
    Vector2D item2D;
    bool queueFull = false;

    static const Color defaultItemColor = Color( 169, 169, 169, 175 );
    static const Color goodItemColor = Color( 255, 0, 255, 200 );
    static const Color corpseColor = Color( 255, 182, 193, 200 );

    LOCALGAMEWORLD gameworld = process->Read<LOCALGAMEWORLD>( gameWorldAddr );

    LootArray itemList = process->Read<LootArray>((uintptr_t)gameworld.m_pItemList );

    for( int i = 0; i < itemList.Count; i++ ){
        uintptr_t itemAddr = process->Read<uintptr_t>((uintptr_t)itemList.m_pItemList + (0x20 + (i * 8)) );

        if( !itemAddr )
            continue;

        const Color *itemColoring = &defaultItemColor;

        // Get Item Location and name
        Item item = process->Read<Item>( itemAddr );
        ItemProfile itemProfile = process->Read<ItemProfile>( (uintptr_t)item.m_pItemProfile );
        ItemBasicInformation itemBasicInformation = process->Read<ItemBasicInformation>( (uintptr_t)itemProfile.m_pItemInformation );
        ItemLocalization itemLocalization = process->Read<ItemLocalization>( (uintptr_t)itemBasicInformation.m_pItemLocalization );
        ItemCoordinates itemCoordinates = process->Read<ItemCoordinates>( (uintptr_t)itemLocalization.m_pItemCoordinates );
        ItemLocationContainer itemLocationContainer = process->Read<ItemLocationContainer>( (uintptr_t)itemCoordinates.m_pItemLocationContainer );

        if( !Unity::World2Screen( itemLocationContainer.ItemPosition, &item2D ) )
            continue;

        process->Read( (uintptr_t)itemBasicInformation.ItemPatName, nameBuffer, 128 );

        if( IsPlayerCorpse( nameBuffer ) ){
            itemColoring = &corpseColor;
            strcpy(nameBuffer, "body");
        } else if( IsGoodLoot( nameBuffer ) ){
            itemColoring = &goodItemColor;
            //strcpy(nameBuffer, "goodie");
        } else {
            distance = localPlayerHead.Distance( itemLocationContainer.ItemPosition );
            if( distance > 50.0f )
                continue;
            nameBuffer[127] = '\0';
        }

        queueFull |= !Peeper::AddCircle( item2D.x, item2D.y, *itemColoring, std::min(distance / 6, 8.0f), 16, 1.0f );
        queueFull |= !Peeper::AddText( item2D.x, item2D.y, *itemColoring, nameBuffer );

        //Logger::Log("Item Name: (%s)\n", nameBuffer );
    }

    if( queueFull )
        Logger::Log("Warning! Draw queue full!\n");
}