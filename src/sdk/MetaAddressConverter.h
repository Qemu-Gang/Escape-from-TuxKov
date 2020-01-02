#pragma once


class MetaAddressConverter
{
    int metaSegmentOffset;
    int metaSegmentSize;
    uintptr_t virtualBase;
public:

    MetaAddressConverter( int metaOffset, int metaSize, uintptr_t virtBase )
    {
        metaSegmentOffset = metaOffset;
        metaSegmentSize = metaSize;
        virtualBase = virtBase;
    }

    int PointerToOffset( uintptr_t ptr, int areaStartOffset )
    {
        return (int) (ptr - virtualBase + areaStartOffset);
    }

    int PointerToOffset( uintptr_t ptr )
    {
        return PointerToOffset( ptr, metaSegmentOffset );
    }

    uintptr_t OffsetToPointer( int offset, int areaStartOffset )
    {
        return (int) (offset - areaStartOffset + virtualBase);
    }
    uintptr_t OffsetToPointer( int offset )
    {
        return OffsetToPointer( offset, metaSegmentOffset );
    }
};