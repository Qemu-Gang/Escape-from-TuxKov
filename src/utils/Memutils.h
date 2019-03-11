#pragma once
#include "../vmread/hlapi/hlapi.h"
#include "Logger.h"

inline uintptr_t GetAbsoluteAddressVm( WinProcess &process, uintptr_t instructionPtr, int offset, int size ) {
    if( !instructionPtr ){
        return 0;
    }
    return instructionPtr + process.Read<int>( instructionPtr + offset ) + size;
}
