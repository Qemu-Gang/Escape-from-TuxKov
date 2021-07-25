#pragma once

#include <cstring>

static inline bool IsPlayerCorpse( const char *string )
{
    // This picks up scav corpses too for some reason
    return !strncmp( string, "Observed_Player", 15 );
}

static inline bool IsGoodLoot( const char *string )
{
    bool isGood = false;
    isGood |= (bool)strstr( string, "_transilluminator" );
    //isGood |= (bool)strstr( string, "key_" );
    isGood |= (bool)strstr( string, "gl40" );
    isGood |= (bool)strstr( string, "lab" );
    isGood |= (bool)strstr( string, "info_intelligence" );
    isGood |= (bool)strstr( string, "video_card" );
    isGood |= (bool)strstr( string, "barter_electr_tetriz" );

    return isGood;
}