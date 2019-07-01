#include "Bhop.h"
#include "../utils/Wrappers.h"
#include "../sdk/CBaseEntity.h"

void Bhop::Bhop( CBaseEntity &localplayer ) {
    if( !(pressedKeys[KEY_SPACE]) ){
        return;
    }

    if( localplayer.GetFlags() & FL_ONGROUND ){
        process->Write<int>( forceJump, 6 );
    }
}