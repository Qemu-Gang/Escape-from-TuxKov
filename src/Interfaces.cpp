#include "Interfaces.h"
#include "utils/Logger.h"
#include "utils/Memutils.h"
#include "sdk/BaseStruct.h"
#include "m0dular/utils/pattern_scan.h"
#include "globals.h"

typedef void* (*InstantiateInterfaceFn) ();

struct InterfaceReg
{
    InstantiateInterfaceFn m_CreateFn;
    const char *m_pName;
    InterfaceReg *m_pNext;
};


bool Interfaces::FindInterfaces( const char *moduleName ) {
    uintptr_t createInterfaceFunc = PatternScan::FindPattern("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC ?? 48 8B 1D ?? ?? ?? ?? 48 8B FA 48", moduleName);
    if( !createInterfaceFunc ){
        return false;
    }
    uintptr_t interfaceList = GetAbsoluteAddressVm( *process, createInterfaceFunc + 20, 3, 7 );

    char buffer[256];
    for( uintptr_t interface = (uintptr_t)process->Read<InterfaceReg*>(interfaceList);
         interface && interface != (uintptr_t)process->Read<InterfaceReg*>( interface + OFFSET_OF(&InterfaceReg::m_pNext) );
         interface = (uintptr_t)process->Read<InterfaceReg*>( interface + OFFSET_OF(&InterfaceReg::m_pNext) )
       ){
        Logger::Log("Interface: %p - ", (void*)interface);
        VMemRead( &process->ctx->process, process->proc.dirBase, (uint64_t)buffer, process->Read<uintptr_t>(interface + OFFSET_OF(&InterfaceReg::m_pName)), 256 );
        Logger::Log("InterfaceName : %s\n", buffer);
    }

    return true;
}
