#include "Scanner.h"
#include "Logger.h"

char *byteCache; // Windows module copied in

inline bool Compare( const unsigned char* pData, const unsigned char* bMask, const char* szMask )
{
    for (; *szMask; ++szMask, ++pData, ++bMask)
        if (*szMask == 'x' && *pData != *bMask)
            return false;

    return (*szMask) == 0;
}

static uintptr_t FindPatternOffset( uintptr_t start, uintptr_t max, unsigned char* bMask, const char* szMask )
{
    for (uintptr_t i = 0; i < max; i++) {
        uintptr_t currAddr = start + i;
        if ( Compare( (unsigned char*)currAddr, bMask, szMask ) ){
            return currAddr;
        }
    }

    return 0;
}

uintptr_t Scanner::FindPatternInModule( const char *pattern, const char *moduleName, WinProcess &process ) {
    size_t patternLen = strlen( pattern );
    uintptr_t startingAddr = 0;
    uintptr_t endingAddr = 0;
    uintptr_t ret = 0;

    unsigned char *bMask = new unsigned char[patternLen](); //  bit bigger than needed but ok
    char *szMask = new char[patternLen](); //  ^^

    unsigned int bMaskIndex = 0;
    unsigned int szMaskIndex = 0;

    for (const auto& o : process.modules) {
        if ( !strcasecmp( moduleName, o.info.name ) ) {
            startingAddr = o.info.baseAddress;
            endingAddr = startingAddr + o.info.sizeOfModule;
        }
    }

    Logger::Log("StartingAddr: %p, EndingAddr: %p\n", (void*)startingAddr, (void*)endingAddr);
    if( !startingAddr || !endingAddr ) {
        Logger::Log("ERROR: No Starting/Ending Addr");
        goto end;
    }

    /* Copy module into linux memory */
    byteCache = new char[endingAddr - startingAddr];
    VMemRead( &process.ctx->process, process.proc.dirBase, (uint64_t)byteCache, startingAddr, endingAddr - startingAddr );


    /* Generate a byte mask and x/? mask at the same time */
    char byteBuffer[2];
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
            bMask[bMaskIndex] = (unsigned char)strtol( byteBuffer, NULL, 16 );
            bMaskIndex++;
        }
    }

    Logger::Log("bMask is: (");
    for( size_t i = 0; i < bMaskIndex; i ++ ){
        Logger::Log("%x,", bMask[i]);
    }
    Logger::Log(")\n");
    Logger::Log("szMask is %s\n", szMask);


    ret = FindPatternOffset( (uintptr_t)byteCache, (uintptr_t)&byteCache[endingAddr - startingAddr], bMask, szMask );
    ret = ret - (uintptr_t)byteCache;

    Logger::Log("Offset is %p\n", (void*)ret);

end:
    delete[] bMask;
    delete[] szMask;
    delete[] byteCache;
    return startingAddr + ret;
}