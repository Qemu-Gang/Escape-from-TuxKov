#pragma once
#include "vmread/hlapi/hlapi.h"

namespace Netvars
{
    bool FindNetvars( WinProcess &process, const char *moduleName );
}

