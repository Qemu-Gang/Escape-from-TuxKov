#pragma once

#include "../vmread/hlapi/hlapi.h"

namespace Scanner
{
    uintptr_t FindPatternInModule(const char* pattern, const char *moduleName, WinProcess &process);

}
