#pragma once
#include "vmread/hlapi/hlapi.h"
#include "Logger.h"

inline uintptr_t GetAbsoluteAddressVm( WinProcess &proc, uintptr_t instructionPtr, int offset, int size ) {
    if( !instructionPtr ){
        return 0;
    }
    return instructionPtr + proc.Read<int>( instructionPtr + offset ) + size;
}
