#include "../sdk/Vector.h"
#include "../sdk/QAngle.h"
#include "../utils/Logger.h"

namespace Math {
    inline QAngle CalcAngle(const Vector &src, const Vector &dst) {
        return QAngle(src - dst);
    }


    inline void Clamp(QAngle &angles) {
        if (angles[YAW] > 180.0f) {
            angles[YAW] = 180.0f;
        } else if (angles[YAW] < -180.0f) {
            angles[YAW] = -180.0f;
        }

        if (angles[PITCH] > 89.0f) {
            angles[PITCH] = 89.0f;
        } else if (angles[PITCH] < -89.0f) {
            angles[PITCH] = -89.0f;
        }
        angles[ROLL] = 0.0f;
    }

    inline float AngleFOV(const QAngle &viewAngle, const QAngle &aimAngle) {
        QAngle delta = viewAngle - aimAngle;
        delta.Normalize();
        float fov = delta.Length();
        if (fov > 180.0f)
            fov -= 360.0f;

        fov = fabsf(fov);
        return fov;
    }

    inline float DistanceFOV(const QAngle &viewAngle, const QAngle &aimAngle, float distance) {
        float angleFOV = AngleFOV(viewAngle, aimAngle);

        angleFOV /= 90.0f;

        float distanceFOV = angleFOV * distance; // sin(dif) = viewAngle / realDelta

        return distanceFOV;
    }

}
