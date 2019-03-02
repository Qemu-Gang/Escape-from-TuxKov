#include "../sdk/Vector.h"
#include "../sdk/QAngle.h"

#define DEG_TO_RADIAN(deg) (deg * M_PI / 180.0f)

namespace Math {
    inline QAngle CalcAngle(Vector &src, Vector &dst) {
        QAngle ret;
        Vector delta = src - dst;
        double hyp = delta.Length2D();
        if (hyp == 0.0f)
            return QAngle();

        ret[PITCH] = (asinf(delta.z / hyp)) * 180.0f / M_PI;
        ret[YAW] = (atanf(delta.y / delta.x)) * 180.0f / M_PI;

        if (delta.x >= 0.0f)
            ret[YAW] += 180.0f;

        return ret;
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
        float fov = sqrtf(delta.x * delta.x + delta.y * delta.y);
        if (fov > 180.0f)
            fov -= 360.0f;

        fov = fabsf(fov);

        return fov;
    }

    inline float DistanceFOV(const QAngle &viewAngle, const QAngle &aimAngle, float distance) {
        float angleFOV = AngleFOV(viewAngle, aimAngle);

        float radian = DEG_TO_RADIAN(angleFOV);
        distance *= 0.01905f;
        float distanceFOV = sinf(radian) * distance; // sin(dif) = viewAngle / realDelta

        return distanceFOV;
    }

}
