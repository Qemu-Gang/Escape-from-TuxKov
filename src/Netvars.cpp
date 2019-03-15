#include "Netvars.h"
#include "utils/Logger.h"
#include "utils/Memutils.h"
#include "sdk/ClientClass.h"
#include "m0dular/utils/pattern_scan.h"
#include "globals.h"

bool Netvars::PrintNetvars( WinProcess &process, const char *moduleName ) {
    uintptr_t clientHeadAddr = PatternScan::FindPattern("[48 8B 1D *?? ?? ?? ??] 48 8B AC 24", moduleName);
    if( !clientHeadAddr ){
        return false;
    }
    uintptr_t recvTable;
    int32_t propNum;
    uint32_t offset;
    PropType propType;
    uintptr_t prop;
    char buffer[256];

    for(uintptr_t cClass = (uintptr_t)process.Read<uintptr_t>(clientHeadAddr);
        cClass;
        cClass = (uintptr_t)process.Read<ClientClass*>( cClass + OFFSET_OF(&ClientClass::next) ) ){

        VMemRead( &process.ctx->process, process.proc.dirBase, (uint64_t)buffer, process.Read<uintptr_t>( cClass + OFFSET_OF( &ClientClass::networkName ) ), 256 );
        Logger::Log( "ClientClass: %p - Network Name: %s\n", (void*)cClass, buffer );
        recvTable = (uintptr_t)process.Read<RecvTable*>( cClass + OFFSET_OF( &ClientClass::recvTable ) );
        propNum = process.Read<int32_t>( recvTable + OFFSET_OF( &RecvTable::numOfProps ) );
        Logger::Log("\tPropNum: %d\n", propNum);

        for( int i = 0; i < propNum; i++ ){
            prop = process.Read<uintptr_t>(process.Read<uintptr_t>( recvTable + OFFSET_OF( &RecvTable::pProps ) ) + (i * sizeof(uintptr_t) ));
            propType = process.Read<PropType>( prop + OFFSET_OF( &RecvProp::dataType ) );
            offset = process.Read<uint32_t>( prop + OFFSET_OF( &RecvProp::offset ) );
            VMemRead( &process.ctx->process, process.proc.dirBase, (uint64_t)buffer, process.Read<uintptr_t>( prop + OFFSET_OF( &RecvProp::name ) ), 256 );
            Logger::Log("\tProp: %s(%s) - @%x\n", buffer, PropType2String( propType ), offset );
        }

    }

    return true;
}

void Netvars::CacheNetvars( const char *moduleName ) {
    uintptr_t clientHeadAddr = PatternScan::FindPattern("[48 8B 1D *?? ?? ?? ??] 48 8B AC 24", moduleName);
    if( !clientHeadAddr ){
        Logger::Log("Failed to get clientHeadAddr");
        return;
    }
    uintptr_t recvTable;
    int32_t propNum;
    uint32_t offset;
    PropType propType;
    uintptr_t prop;
    char className[256];
    char propName[256];

    for(uintptr_t cClass = (uintptr_t)process->Read<uintptr_t>(clientHeadAddr);
        cClass;
        cClass = (uintptr_t)process->Read<ClientClass*>( cClass + OFFSET_OF(&ClientClass::next) ) ){

        VMemRead( &process->ctx->process, process->proc.dirBase, (uint64_t)className, process->Read<uintptr_t>( cClass + OFFSET_OF( &ClientClass::networkName ) ), 256 );
        recvTable = (uintptr_t)process->Read<RecvTable*>( cClass + OFFSET_OF( &ClientClass::recvTable ) );
        propNum = process->Read<int32_t>( recvTable + OFFSET_OF( &RecvTable::numOfProps ) );

        for( int i = 0; i < propNum; i++ ){
            prop = process->Read<uintptr_t>(process->Read<uintptr_t>( recvTable + OFFSET_OF( &RecvTable::pProps ) ) + (i * sizeof(uintptr_t) ));
            offset = process->Read<uint32_t>( prop + OFFSET_OF( &RecvProp::offset ) );
            VMemRead( &process->ctx->process, process->proc.dirBase, (uint64_t)propName, process->Read<uintptr_t>( prop + OFFSET_OF( &RecvProp::name ) ), 256 );
            netvars[className][propName] = offset;
        }
    }
    Logger::Log("Netvars Cached.\n");
}