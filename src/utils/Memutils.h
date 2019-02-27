#pragma once
#include "../vmread/hlapi/hlapi.h"
#include "Logger.h"

inline uintptr_t GetAbsoluteAddressVm( WinProcess &process, uintptr_t instructionPtr, int offset, int size ) {
    Logger::Log("Int ptr: %p\n", (void*)instructionPtr );
    Logger::Log("Int offset: %p\n", (void*)process.Read<int>( instructionPtr + offset ) );
    return instructionPtr + process.Read<int>( instructionPtr + offset ) + size;
}