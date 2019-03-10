#pragma once

#include "../utils/Math.h"
#include "../utils/Wrappers.h"

namespace Aimbot {
    void RecoilCompensation(QAngle &angle);

    void NoSpread(uintptr_t weapon);

    void SwayCompensation(QAngle &viewAngle, QAngle &angle);

    void Aimbot();

};
