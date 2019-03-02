#ifndef APE_EX_AIMBOT_H
#define APE_EX_AIMBOT_H


#include "../utils/Math.h"
#include "../utils/Wrappers.h"

namespace Aimbot {
    void RecoilCompensation(QAngle &angle);
    void Nospread(uintptr_t weapon);

    void BreathCompensation(FILE*out, QAngle &viewAngle);

    void Aimbot(FILE* out);

};


#endif //APE_EX_AIMBOT_H
