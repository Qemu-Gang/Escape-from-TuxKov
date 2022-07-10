#include "unity.h"

#include "../globals.h"
#include "../utils/Logger.h"

#include <csignal>
#include <immintrin.h>
#include <ios>
#include <iostream>
#include <wchar.h>
#include <uchar.h>
#include <stdio.h>
#include <stdlib.h>

void Unity::PrintGOMObjects( bool tagged ) {
    uintptr_t firstObjectPtr;
    uintptr_t lastObjectPtr;

    if( tagged ){
        firstObjectPtr = (uintptr_t)gom.taggedFirst;
        lastObjectPtr = (uintptr_t)gom.taggedLast;
    } else {
        firstObjectPtr = (uintptr_t)gom.activeFirst;
        lastObjectPtr = (uintptr_t)gom.activeLast;
    }

    uintptr_t itr = firstObjectPtr;

    while( itr ){
        mono_object_wrapper wrapper = process->Read<mono_object_wrapper>( itr );
        mono_object object = process->Read<mono_object>( (uintptr_t)wrapper.object );
        char name[256] = {0};
        process->Read( (uint64_t)object.objectname, name, 256 );

        Logger::Log("[%d][%s]\n", object.Tag, name);

        if( itr == lastObjectPtr )
            break;

        itr = (uintptr_t)wrapper.next;
    }
}

void Unity::PrintPlayerList() {
    LOCALGAMEWORLD gameworld = process->Read<LOCALGAMEWORLD>( gameWorldAddr );

    Array playerList = process->Read<Array>( (uintptr_t)gameworld.m_pPlayerList );

    Logger::Log("Printing %d players..\n", playerList.Count);
    for( int i = 0; i < playerList.Count; i++ ){
        uintptr_t playerAddr = process->Read<uintptr_t>((uintptr_t)playerList.m_pList + (0x20 + (i * 8)));

        if( !playerAddr )
            continue;

        Player player = process->Read<Player>( playerAddr );
        PlayerProfile profile = process->Read<PlayerProfile>( (uintptr_t)player.m_pPlayerProfile );
        PlayerInfo info = process->Read<PlayerInfo>( (uintptr_t)profile.m_PlayerInfo );
        UnityEngineString name = process->Read<UnityEngineString>( (uintptr_t)info.m_pPlayerName );

        Logger::Log("Player Addr(%p) - healthcontroller(%p) Side(%d)\n", playerAddr, player.m_pHealthController, info.Side);

        char playername[64] = { 0 };
        if( info.Side == 4 )
        {
            if( info.CreationDate > 0 )
            {
                strcpy(playername, "player-scav");
            }
            else
            {
                continue;
            }
        }
        else
        {
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
        }

        float hp = 0;
        auto healthController = process->Read<HealthController>( (uintptr_t)player.m_pHealthController );
        auto healthBody = process->Read<HealthBody>( (uintptr_t)healthController.m_pHealthBody );
        auto bodyController = process->Read<BodyController>( (uintptr_t)healthBody.m_pBodyController );

        // Calculate HP
        for( int limb = 0; limb < NUM_BODY_PARTS; limb++ )
        {
            HealthContainer bodypart = process->Read<HealthContainer>( (uintptr_t)bodyController.m_bodyParts[limb].m_pBodyPart );
            Health bodypartHealth = process->Read<Health>( (uintptr_t)bodypart.m_pHealth );
            hp += bodypartHealth.Health;
        }

        Vector3D headPos = Unity::GetBonePosition( playerAddr, Bones::HumanHead );
        if( player.m_pLocalPlayerChecker ){
            Logger::Log("[LOCALPLAYER-Team(%d)][%d](%s)Player(%f,%f,%f) - HP(%f)\n", info.Side, name.size, playername, headPos.x, headPos.y, headPos.z, hp);
        } else {
            Logger::Log("[Team(%d)](%s)Player(%f,%f,%f) - HP(%f)\n", info.Side, playername, headPos.x, headPos.y, headPos.z, hp);
        }
    }
    Logger::Log("Done printing players.\n");
}

void Unity::PrintItemStats() {
    LOCALGAMEWORLD gameworld = process->Read<LOCALGAMEWORLD>( gameWorldAddr );

    LootArray itemList = process->Read<LootArray>((uintptr_t)gameworld.m_pItemList );

    Logger::Log("This map has %d Items\n", itemList.Count);
}

uint64_t matrix_list_base;
uint64_t dependency_index_table_base = 0;

static Vector3D GetPosition(uint64_t transform)
{
    auto transform_internal = process->Read<uint64_t>(transform + 0x10);

    auto matrices = process->Read<uint64_t>(transform_internal + 0x38);
    auto index = process->Read<int>(transform_internal + 0x40);

    process->Read((uintptr_t)(matrices + 0x18), &matrix_list_base, sizeof(matrix_list_base));
    process->Read((uintptr_t)(matrices + 0x20), &dependency_index_table_base, sizeof(dependency_index_table_base));

    static auto get_dependency_index = [](uint64_t base, int32_t index)
    {
        process->Read((uintptr_t)(base + index * 4), &index, sizeof(index));
        return index;
    };

    static auto get_matrix_blob = [](uint64_t base, uint64_t offs, float* blob, uint32_t size) {
        process->Read((uintptr_t)(base + offs), blob, size);
    };

    int32_t index_relation = get_dependency_index(dependency_index_table_base, index);

    Vector3D ret_value;
    {
        float* base_matrix3x4 = (float*)malloc(64),
                * matrix3x4_buffer0 = (float*)((uint64_t)base_matrix3x4 + 16),
                * matrix3x4_buffer1 = (float*)((uint64_t)base_matrix3x4 + 32),
                * matrix3x4_buffer2 = (float*)((uint64_t)base_matrix3x4 + 48);

        get_matrix_blob(matrix_list_base, index * 48, base_matrix3x4, 16);

        __m128 xmmword_1410D1340 = { -2.f, 2.f, -2.f, 0.f };
        __m128 xmmword_1410D1350 = { 2.f, -2.f, -2.f, 0.f };
        __m128 xmmword_1410D1360 = { -2.f, -2.f, 2.f, 0.f };

        while (index_relation >= 0)
        {
            uint32_t matrix_relation_index = 6 * index_relation;

            // paziuret kur tik 3 nureadina, ten translationas, kur 4 = quatas ir yra rotationas.
            get_matrix_blob(matrix_list_base, 8 * matrix_relation_index, matrix3x4_buffer2, 16);
            __m128 v_0 = *(__m128*)matrix3x4_buffer2;

            get_matrix_blob(matrix_list_base, 8 * matrix_relation_index + 32, matrix3x4_buffer0, 16);
            __m128 v_1 = *(__m128*)matrix3x4_buffer0;

            get_matrix_blob(matrix_list_base, 8 * matrix_relation_index + 16, matrix3x4_buffer1, 16);
            __m128i v9 = *(__m128i*)matrix3x4_buffer1;

            __m128* v3 = (__m128*)base_matrix3x4; // r10@1
            __m128 v10; // xmm9@2
            __m128 v11; // xmm3@2
            __m128 v12; // xmm8@2
            __m128 v13; // xmm4@2
            __m128 v14; // xmm2@2
            __m128 v15; // xmm5@2
            __m128 v16; // xmm6@2
            __m128 v17; // xmm7@2

            v10 = _mm_mul_ps(v_1, *v3);
            v11 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 0));
            v12 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 85));
            v13 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -114));
            v14 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -37));
            v15 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -86));
            v16 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 113));

            v17 = _mm_add_ps(
                    _mm_add_ps(
                            _mm_add_ps(
                                    _mm_mul_ps(
                                            _mm_sub_ps(
                                                    _mm_mul_ps(_mm_mul_ps(v11, (__m128)xmmword_1410D1350), v13),
                                                    _mm_mul_ps(_mm_mul_ps(v12, (__m128)xmmword_1410D1360), v14)),
                                            _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), -86))),
                                    _mm_mul_ps(
                                            _mm_sub_ps(
                                                    _mm_mul_ps(_mm_mul_ps(v15, (__m128)xmmword_1410D1360), v14),
                                                    _mm_mul_ps(_mm_mul_ps(v11, (__m128)xmmword_1410D1340), v16)),
                                            _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 85)))),
                            _mm_add_ps(
                                    _mm_mul_ps(
                                            _mm_sub_ps(
                                                    _mm_mul_ps(_mm_mul_ps(v12, (__m128)xmmword_1410D1340), v16),
                                                    _mm_mul_ps(_mm_mul_ps(v15, (__m128)xmmword_1410D1350), v13)),
                                            _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 0))),
                                    v10)),
                    v_0);

            *v3 = v17;

            index_relation = get_dependency_index(dependency_index_table_base, index_relation);
        }

        ret_value = *(Vector3D*)base_matrix3x4;
        delete[] base_matrix3x4;
    }

    return ret_value;
}

Vector3D Unity::GetBonePosition( uintptr_t playerAddr, int boneID ) {
    // The power of external!
    Player player = process->Read<Player>( playerAddr );
    PlayerBody playerBody = process->Read<PlayerBody>( (uintptr_t)player.m_pPlayerBody );
    m_pSkeletonRootJoin skeletonRootJoin = process->Read<m_pSkeletonRootJoin>( (uintptr_t)playerBody.m_pSkeletonRootJoin );
    BoneEnumerator bones = process->Read<BoneEnumerator>( (uintptr_t)skeletonRootJoin.m_pBoneEnumerator );

    uintptr_t transform = process->Read<uintptr_t>( uintptr_t(bones.m_pTransformArray) + (0x20 + (boneID * 8) ) );
    return GetPosition( transform );
}

uintptr_t Unity::GetObjectPtrByName(const char *objname, bool tagged) {
    uintptr_t firstObjectPtr;
    uintptr_t lastObjectPtr;

    if( tagged ){
        firstObjectPtr = (uintptr_t)gom.taggedFirst;
        lastObjectPtr = (uintptr_t)gom.taggedLast;
    } else {
        firstObjectPtr = (uintptr_t)gom.activeFirst;
        lastObjectPtr = (uintptr_t)gom.activeLast;
    }

    uintptr_t itr = firstObjectPtr;

    while( itr ){
        mono_object_wrapper wrapper = process->Read<mono_object_wrapper>( itr );
        mono_object object = process->Read<mono_object>( (uintptr_t)wrapper.object );
        char name[256] = {0};
        process->Read( (uint64_t)object.objectname, name, 256 );

        if( !strcmp( objname, name ) ){
            game_object_wrapper gameobjWrapper = process->Read<game_object_wrapper>( (uintptr_t)object.pObjectClass );
            return (uintptr_t)gameobjWrapper.gameObject;
        }
        if( itr == lastObjectPtr )
            break;

        itr = (uintptr_t)wrapper.next;
    }

    return 0;
}

uintptr_t Unity::GetWorldPtr() {
    uintptr_t firstObjectPtr;
    uintptr_t lastObjectPtr;

    firstObjectPtr = (uintptr_t)gom.activeFirst;
    lastObjectPtr = (uintptr_t)gom.activeLast;

    uintptr_t itr = firstObjectPtr;

    while( itr ){
        mono_object_wrapper wrapper = process->Read<mono_object_wrapper>( itr );
        mono_object object = process->Read<mono_object>( (uintptr_t)wrapper.object );
        char name[256] = {0};
        process->Read( (uint64_t)object.objectname, name, 256 );

        if( !strcmp( "GameWorld", name ) ){
            game_object_wrapper gameobjWrapper = process->Read<game_object_wrapper>( (uintptr_t)object.pObjectClass );
            GameWorldWrapper gameWorldWrapper = process->Read<GameWorldWrapper>( (uintptr_t)gameobjWrapper.gameObject );
            LOCALGAMEWORLD world = process->Read<LOCALGAMEWORLD>( (uintptr_t)gameWorldWrapper.localgameworld );
            Array playerList = process->Read<Array>( (uintptr_t)world.m_pPlayerList );
            if( playerList.Count == 0 )
                goto next;

            return (uintptr_t)gameWorldWrapper.localgameworld;
        }
        if( itr == lastObjectPtr )
            break;

        next:
        itr = (uintptr_t)wrapper.next;
    }

    return 0;
}

bool Unity::World2Screen(const Vector3D &world, Vector2D *screen) {
    Matrix4x4 matrix = cameraEntity.ViewMatrix.Transpose();

    const Vector3D translation = { matrix.m[3][0], matrix.m[3][1], matrix.m[3][2] };
    const Vector3D up = { matrix.m[1][0], matrix.m[1][1], matrix.m[1][2] };
    const Vector3D right = { matrix.m[0][0], matrix.m[0][1], matrix.m[0][2] };

    const float w = translation.DotProduct( world ) + matrix.m[3][3];

    // Not on screen
    if( w < 0.1f )
        return false;

    float x = right.DotProduct( world ) + matrix.m[0][3];
    float y = up.DotProduct( world ) + matrix.m[1][3];

    if (localPlayerIsAiming && localPlayerIsOpticCamera)
    {
        //TODO: scope scale
    }

    screen->x = (2560 / 2) * (1.f + x / w);
    screen->y = (1440 / 2) * (1.f - y / w);

    return true;
}