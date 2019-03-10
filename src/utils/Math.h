#include "../sdk/Vector.h"
#include "../sdk/QAngle.h"
#include "../utils/Logger.h"

#define DEG2RAD(deg) (deg * M_PI / 180.0f)

namespace Math {
    inline QAngle CalcAngle(Vector &src, Vector &dst) {
        QAngle angles;
        Vector delta = src - dst;

        if (delta.x == 0.0f && delta.y == 0.0f) {
            angles[PITCH] = (delta[2] > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
            angles[YAW] = 0.0f; //yaw left/right
        } else {
            angles[PITCH] = (atan2(-delta.z, delta.Length2D())) * -180 / M_PI;

            angles[YAW] = (atan2(delta.y, delta.x)) * 180 / M_PI;

            if (angles[YAW] > 90)
                angles[YAW] -= 180;
            else if (angles[YAW] < 90)
                angles[YAW] += 180;
            else if (angles[YAW] == 90)
                angles[YAW] = 0;
        }
        angles[ROLL] = 0.0f;
        return angles;
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

        if (angleFOV >= 180.0f)
            angleFOV -= 0.2f;

        float radian = DEG2RAD(angleFOV);

        float distanceFOV = sinf(radian) * distance; // sin(dif) = viewAngle / realDelta

        return distanceFOV;
    }

}
