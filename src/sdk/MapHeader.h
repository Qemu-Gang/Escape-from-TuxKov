#include <cstdint>
#include <utility>

#include "SectionPointerConverter.h"
#include "MetaAddressConverter.h"
#include "tags/TagBase.h"

enum class MapFileType : int16_t
{
    SINGLEPLAYER = 0,
    MULTIPLAYER,
    MAINMENU,
    SHARED,
    SINGLEPLAYERSHARED,
};

struct haloInt
{
    int32_t value;
    char signature[4]; // literally the ascii values "343i"
};

class ImportantTagTable;
class PolyArtFixupTable;

class TagTableEntry
{
public:
    int16_t classIndex;
    uint16_t datumIndexSalt;
    uint32_t memoryAddressShortPtr;
};

class TagTable
{
public:
    TagTableEntry entries[999]; // lots
};


class ClassTableEntry
{
public:
    int32_t magic;
    int32_t parentMagic;
    int32_t grandparentMagic;
    uint32_t stringID;
};

class ClassTable
{
public:
    ClassTableEntry entries[999];
};

class IndexHeader
{
public:
    haloInt numClasses;
    ClassTable *classTable;

    haloInt numTags;
    TagTable *tagTable;

    haloInt numImportantTags;
    ImportantTagTable *importantTagTable;

    haloInt numPolyArtFixups;
    PolyArtFixupTable *polyArtFixupTable;

    void *_pad;
    uint32_t magic; // should be "sgat" or 0x74616773 - tags backwards
};

class MapPartition
{
public:
    uintptr_t loadAddr; //0x0000
    uint64_t size; //0x0008
}; //Size: 0x0010

class HaloMapHeader
{
public:
    void* virtualBaseAddress_toMetaArea; //0x0000
    char pad_0008[8]; //0x0008
    uint32_t header; //0x0010 -- 'daeh' or 0x68656164
    int32_t version; //0x0014
    uint64_t fileSize; //0x0018
    IndexHeader* indexHeader; //0x0020
    uint32_t tagBufferOffset; //0x0028
    uint32_t tagBufferSize; //0x002C
    char mapName[256]; //0x0030
    char engineBuildDate[32]; //0x0130
    MapFileType mapFileType; //0x0150
    char pad_0152[26]; //0x0152
    uint32_t stringDataTableCount; //0x016C
    uint32_t stringDataTableSize; //0x0170
    uint32_t stringIndexTableOffset; //0x0174
    uint32_t stringDataTableOffset; //0x0178
    char pad_017C[44]; //0x017C
    char internalName[36]; //0x01A8
    char scenarioName[260]; //0x01CC
    uint32_t fileNameTableCount; //0x02D0
    uint32_t fileNameTableOffset; //0x02D4
    uint32_t fileNameTableSize; //0x02D8
    int32_t fileIndexTableOffset; //0x02DC
    char pad_02E0[40]; //0x02E0
    uintptr_t virtualBaseAddr; //0x0308
    char pad_0310[8]; //0x0310
    MapPartition partitions[6]; //0x0318
    char pad_0378[332]; //0x0378
    uint32_t sectionMasks[4]; //0x04C4
    MapSection sections[4]; //0x04D4
    char pad_04F4[39704]; //0x04F4
    uint32_t footer; //0xA00C - 0x666F6F74 - "toof"

    /// Post Header in-memory content
    uintptr_t addressMask; //0xA010 - points to the indexHeader above
    uintptr_t memoryBuffers[4];
    char pad_A038[8]; //0xA038
    void* N000015B9; //0xA040
    uintptr_t stringDataTable; //0xA048 -- Mustard, Sleepy, Sketch, Crazy...
    char pad_A050[80]; //0xA050
    uintptr_t metaMagick; //0xA0A0
    uintptr_t fileNameData; //0xA0A8 -- i've got a lovely bunch of coconuts, there they are standing in a row, (then a bunch of filepaths)
    char pad_A0B0[8]; //0xA0B0
    uintptr_t fileNameIndexTable; //0xA0B8 -- offsets for the fileNameData table. Not really needed since they are null-terminated.
    char pad_A0C0[8]; //0xA0C0
    uintptr_t fileNameData2; //0xA0C8 - same as the one above
    char pad_A0D0[272]; //0xA0D0
};

constexpr int mapHeaderSize = 0xA000;
constexpr int memorySpaceBuffer = 0x101000;