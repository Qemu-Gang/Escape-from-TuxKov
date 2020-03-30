#include "esp.h"

#include "../globals.h"
#include "../sdk/unity.h"
#include "peeper/client/peeper.h"
#include "../utils/Logger.h"

#include <csignal>

const int arm_right[] = { Bones::HumanRUpperarm, Bones::HumanRForearm1, Bones::HumanRPalm };
const int arm_left[] = { Bones::HumanLUpperarm, Bones::HumanLForearm1, Bones::HumanLPalm };

const int leg_right[] = { Bones::HumanRThigh1, Bones::HumanRCalf, Bones::HumanRFoot };
const int leg_left[] = { Bones::HumanLThigh1, Bones::HumanLCalf, Bones::HumanLFoot };

void ESP::DrawPlayers( ) {
    LOCALGAMEWORLD gameworld = process->Read<LOCALGAMEWORLD>( gameWorldAddr );

    Array playerList = process->Read<Array>( (uintptr_t)gameworld.m_pPlayerList );

    // Garbage data.
    if( playerList.Count > 100 || playerList.Count < 0 )
        return;

    for( int i = 0; i < playerList.Count; i++ ){
        Vector2D head2D, neck2D, pelvis2D;
        char name[256];
        UnityEngineString playerName;
        Color color;
        Player player;
        PlayerProfile profile;
        PlayerInfo info;
        uintptr_t playerAddr = process->Read<uintptr_t>((uintptr_t)playerList.m_pList + (0x20 + (i * 8)));

        if( !playerAddr )
            continue;

        player = process->Read<Player>( playerAddr );

        // Localplayer
        if( player.m_pLocalPlayerChecker ){
            localPlayerAddr = playerAddr;
            continue;
        }


        profile = process->Read<PlayerProfile>( (uintptr_t)player.m_pPlayerProfile );
        info = process->Read<PlayerInfo>( (uintptr_t)profile.m_PlayerInfo );
        //playerName = process->Read<UnityEngineString>( (uintptr_t) info.m_pPlayerName );

        // Scav check
        if( info.CreationDate > 0 ){
            color = Color( 225, 0, 0, 255 );
        } else {
            color = Color( 255, 179, 71, 255 );
        }

        Vector3D headPos = Unity::GetBonePosition( playerAddr, Bones::HumanHead );
        Vector3D neckPos = Unity::GetBonePosition( playerAddr, Bones::HumanNeck );
        Vector3D pelvisPos = Unity::GetBonePosition( playerAddr, Bones::HumanPelvis );

        // Needed bones for anything
        if( !Unity::World2Screen( headPos, &head2D )
        || !Unity::World2Screen( neckPos, &neck2D )
        || !Unity::World2Screen( pelvisPos, &pelvis2D ) ){
            continue;
        }

        // Draw name/distance
        if( localPlayerAddr ){
            Vector3D localHead = Unity::GetBonePosition( localPlayerAddr, Bones::HumanHead );
            sprintf(name, "%0.2fm", playerName.name, localHead.Distance( headPos ) );
            Peeper::AddText( head2D.x, head2D.y, color, name );
        }

        // Draw Spine
        Peeper::AddLine( head2D.x, head2D.y, neck2D.x, neck2D.y, color, 1.0f );
        Peeper::AddLine( neck2D.x, neck2D.y, pelvis2D.x, pelvis2D.y, color, 1.0f );

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