#include "m0dular/utils/memutils.h"
#include "../globals.h"

//TODO: Add thread-local process selection
void ReadMem(void* destination, void* source, size_t size)
{
    process->Read((uint64_t)source, destination, size);
}

void WriteMem(void* destination, void* source, size_t size)
{
    process->Write((uint64_t)destination, source, size);
}
