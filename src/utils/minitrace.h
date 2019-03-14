#pragma once

#define MTR_SCOPED
#include "m0dular/submodules/minitrace/minitrace.h"
#ifdef MTR_ENABLED
#define MTR_SCOPED_TRACE(cat, name) auto scopedTrace = MTRScopedTrace(cat, name);
#else
#define MTR_SCOPED_TRACE(cat, name)
#endif
