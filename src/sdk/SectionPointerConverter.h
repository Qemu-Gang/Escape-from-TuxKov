#pragma once

enum MapSectionType
{
    SECTION_DEBUG = 0,
    SECTION_RESOURCE,
    SECTION_TAG,
    SECTION_LOCALIZATION
};

class MapSection
{
public:
    uint32_t virtualAddress; //0x0000
    uint32_t size; //0x0004
}; //Size: 0x0008

class SectionPointerConverter
{
public:
    int defaultStartOffset;
    uintptr_t basePointer;

    SectionPointerConverter( const MapSection &section, const uint32_t sectionMask )
    {
        basePointer = section.virtualAddress;
        defaultStartOffset = section.virtualAddress + sectionMask;
    }

    int PointerToOffset( uintptr_t pointer )
    {
        return (int) (pointer - basePointer + defaultStartOffset );
    }

    int PointerToOffset( uintptr_t pointer, int startOffset )
    {
        return (int) (pointer - basePointer + startOffset );
    }

    uintptr_t OffsetToPointer( int offset )
    {
        return (uintptr_t) (offset - defaultStartOffset + basePointer );
    }

    uintptr_t OffsetToPointer( int offset, int startOffset )
    {
        return (uintptr_t) (offset - startOffset + basePointer );
    }
};