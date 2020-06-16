#include "../sdk/Vector.h"
#include "../utils/Logger.h"

#define RAD2DEG 57.295779513f

namespace Math {

    inline float AngleFOV(const QAngle &viewAngle, const QAngle &aimAngle) {
        QAngle delta = viewAngle - aimAngle;
        return fabsf(delta.Length());
    }

    inline float DistanceFOV(const QAngle &viewAngle, const QAngle &aimAngle, const float distance) {
        float angleFOV = AngleFOV(viewAngle, aimAngle);

        angleFOV /= 90.0f;

        float distanceFOV = angleFOV * distance;

        return distanceFOV;
    }
}
