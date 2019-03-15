#pragma once
#include "vmread/hlapi/hlapi.h"

#include <unordered_map>

namespace Netvars
{
    bool PrintNetvars( WinProcess &process, const char *moduleName );
    void CacheNetvars( const char *moduleName );

    inline std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>> netvars;
}

