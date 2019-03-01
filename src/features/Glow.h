#ifndef APE_EX_GLOW_H
#define APE_EX_GLOW_H

#include "../utils/Wrappers.h"


namespace Glow {
    void Glow(float colors[3], FILE* out);
    void GlowPlayer(uintptr_t entity, float colors[3]);
};


#endif //APE_EX_GLOW_H
