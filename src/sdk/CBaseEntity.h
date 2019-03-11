#pragma once
#include "BaseStruct.h"
#include "OffPtr.h"

#define CBASE_ENTITY_OFFSETS(HANDLER)           \
    HANDLER(Vector, 0x1b0, glowCol)             \
    HANDLER(float, 0x2b0, glowTimes1)           \
    HANDLER(float, 0x2b4, glowTimes2)           \
    HANDLER(float, 0x2b8, glowTimes3)           \
    HANDLER(float, 0x2bc, glowTimes4)           \
    HANDLER(float, 0x2c0, glowTimes5)           \
    HANDLER(float, 0x2c4, glowTimes6)           \
    HANDLER(float, 0x2dc, glowDistance)         \
    HANDLER(int, 0x2f0, iGlowEnable)            \
    HANDLER(bool, 0x380, bGlowEnable)

#define CONSTRUCTOR_HANDLER(type, offset, name) , name(baseClass)
#define DEFINE_HANDLER(type, offset, name) OffPtr<type, offset> name;
#define WRITE_BACK_HANDLER(type, offset, name) name.WriteBack(writeList);

class CBaseEntity
{
  private:
    char rBuf[0x800];
    ProcessBaseClass baseClass;
  public:

    CBaseEntity(uintptr_t addr = 0)
        : baseClass(rBuf, addr) CBASE_ENTITY_OFFSETS(CONSTRUCTOR_HANDLER)
    {
    }

    void Update()
    {
        process->Read(baseClass.address, rBuf, sizeof(rBuf));
    }

    void WriteBack(WriteList& writeList)
    {
        CBASE_ENTITY_OFFSETS(WRITE_BACK_HANDLER);
    }

    CBASE_ENTITY_OFFSETS(DEFINE_HANDLER)
};
