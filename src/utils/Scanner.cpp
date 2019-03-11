#include "Scanner.h"
#include "Logger.h"

static unsigned char *byteCache; // Windows module copied in

inline bool Compare( const unsigned char* pData, const unsigned char* bMask, const char* szMask )
{
    for (; *szMask; ++szMask, ++pData, ++bMask)
        if (*szMask == 'x' && *pData != *bMask)
            return false;

    return (*szMask) == 0;
}

static uintptr_t FindPatternOffset( uintptr_t start, uintptr_t max, unsigned char* bMask, const char* szMask )
{
    for (uintptr_t i = start; i < max; i++) {
        if ( Compare( (unsigned char*)i, bMask, szMask ) ){
            return i;
        }
    }

    return 0;
}

uintptr_t Scanner::FindPatternInModule( const char *pattern, const char *moduleName, WinProcess &process ) {
    size_t patternLen = strlen( pattern );
    uintptr_t startingAddr = 0;
    uintptr_t endingAddr = 0;
    uintptr_t offset = 0;
    size_t regionSize = 0;
    unsigned char *bMask = new unsigned char[patternLen](); //  bit bigger than needed but ok
    char *szMask = new char[patternLen](); //  ^^

    char byteBuffer[3] = { '\0', '\0', '\0' };

    unsigned int bMaskIndex = 0;
    unsigned int szMaskIndex = 0;

    for (const auto& o : process.modules) {
        if ( !strcasecmp( moduleName, o.info.name ) ) {
            startingAddr = o.info.baseAddress;
            endingAddr = startingAddr + o.info.sizeOfModule;
        }
    }

    if( !startingAddr || !endingAddr ) {
        Logger::Log("ERROR: No Starting/Ending Addr");
        goto end;
    }

    /* Copy module into linux memory */
    regionSize = endingAddr - startingAddr;
    byteCache = new unsigned char[regionSize];
    VMemRead( &process.ctx->process, process.proc.dirBase, (uint64_t)byteCache, startingAddr, regionSize );

    /* Generate a byte mask and x/? mask at the same time */
    for( size_t i = 0; i < patternLen; i++ ){
        if( pattern[i] == ' ' ){
            szMaskIndex++;
            continue;
        }
        if( pattern[i] == '?' ) {
            szMask[szMaskIndex] = '?';
        } else {
            szMask[szMaskIndex] = 'x';
        }

        // End of word
        if( pattern[i+1] == ' ' || pattern[i+1] == '\0' ){
            byteBuffer[0] = pattern[i - 1];
            byteBuffer[1] = pattern[i];
            bMask[bMaskIndex] = (unsigned char)strtoul( byteBuffer, nullptr, 16 );
            bMaskIndex++;
        }
    }

    /* Do the Scan in linux memory, subtract offset and apply */
    offset = FindPatternOffset( (uintptr_t)byteCache, (uintptr_t)&byteCache[regionSize], bMask, szMask );
    if( !offset ){
        return 0;
    }
    offset = offset - (uintptr_t)byteCache;
end:
    delete[] bMask;
    delete[] szMask;
    delete[] byteCache;
    return startingAddr + offset;
}
