#pragma once

#include <cstring>

static inline bool IsPlayerCorpse( const char *string )
{
    // This picks up scav corpses too for some reason
    return !strcmp( string, "Observed_PlayerSuperior(Clone)" );
}