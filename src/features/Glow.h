#pragma once
#include <cstdint>
#include "../sdk/Vector.h"

class CBaseEntity;

namespace Glow {
    void Glow();

    void GlowPlayer(CBaseEntity &entity, Vector &colors);
}
