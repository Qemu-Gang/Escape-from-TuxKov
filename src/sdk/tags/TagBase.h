#pragma once

inline uintptr_t ExpandPointer( uint32_t shortPtr )
{
    return ( uintptr_t(shortPtr) << 2 ) + 0x50000000;
}

inline uint32_t ShortenPointer( uintptr_t pointer )
{
    return (uint32_t)((pointer - 0x50000000) >> 2);
}

// for Tags -- salt is in tag, ****index arg is i value while iterating tags in tagTable!****
inline uint32_t DatumIndexToValue( uint16_t salt, uint16_t index )
{
    if( salt == 0xFFFF )
        return 0xFFFFFFFF;
    return (uint32_t) (( salt << 16 ) | index);
}

inline std::pair<uint16_t,uint16_t> ValueToDatumIndex( uint32_t value )
{
    uint16_t salt = (uint16_t)((value >> 16) & 0xFFFF);
    uint16_t index = (uint16_t)(value & 0xFFFF);
    return std::make_pair( salt, index );
}

inline const char *AllocClassMagicToString( int32_t magic )
{
    if( magic == 0xFFFFFFFF )
        return nullptr;

    int index = 4;
    char *buffer = new char[5];

    while( magic > 0 ){
        index--;
        buffer[index] = (char)(magic & 0xFF);
        magic >>= 8;
    }

    if( index == 4 ){
        delete buffer;
        return nullptr;
    }

    buffer[4] = '\0';
    return buffer;
}

class TagRef
{
public:
    int32_t classMagic; //0x0000
    char pad_0004[8]; //0x0004
    uint32_t datumIndex; //0x000C
}; //Size: 0x0010


/* Known as "Reflexive" in the halo assembly xml files. */
/* "Reflexive" is basically a variable length array. However, they(assembly) use the term for both arrays in-place and array pointers */
template <typename T>
class SubTagListRef
{
public:
    int32_t numEntries; //0x0000
    uint32_t listShortAddr; //0x0004
    char pad_0008[4]; //0x0008
    T* GetTagOffset()
    {
        return (T*)ExpandPointer( this->listShortAddr );
    }
}; //Size: 0x000C

// placeholder for unknown tag layouts
class UnknownTag
{};