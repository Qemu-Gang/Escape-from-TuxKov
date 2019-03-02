#pragma once

#include "../utils/Math.h"
#include "../utils/Wrappers.h"

namespace Aimbot {
    void RecoilCompensation(QAngle &angle);

    void Nospread(uintptr_t weapon);

    void BreathCompensation(QAngle &viewAngle);

    void Aimbot();

};
