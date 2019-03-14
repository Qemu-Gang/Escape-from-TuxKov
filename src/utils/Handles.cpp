#include "m0dular/utils/handles.h"
#include "../globals.h"

ModuleInfo Handles::GetModuleInfo(const char *module)
{
    WinDll *moduleInfo = process->GetModuleInfo(module);

    if (!moduleInfo)
        return { nullptr, 0, 0 };

    return { (void *)moduleInfo, moduleInfo->info.baseAddress, moduleInfo->info.sizeOfModule };
}
