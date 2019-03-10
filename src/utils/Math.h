#include "../sdk/Vector.h"
#include "../sdk/QAngle.h"
#include "../utils/Logger.h"

#define DEG2RAD(deg) (deg * M_PI / 180.0f)

namespace Math {
    inline QAngle CalcAngle(Vector &src, Vector &dst) {
        QAngle angles;
        Vector delta = src - dst;

        if (delta.x == 0.0f && delta.y == 0.0f) {
            angles[0] = (delta[2] > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
            angles[1] = 0.0f; //yaw left/right
        } else {
            angles[PITCH] = (atan2(-delta.z, delta.Length2D())) * -180 / M_PI;

            angles[YAW] = (atan2(delta.y, delta.x)) * 180 / M_PI;

            if (angles[1] > 90)
                angles[1] -= 180;
            else if (angles[1] < 90)
                angles[1] += 180;
            else if (angles[1] == 90)
                angles[1] = 0;
        }
        angles[2] = 0.0f;
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
        float fov = sqrtf(delta.x * delta.x + delta.y * delta.y);
        if (fov > 180.0f)
            fov -= 360.0f;

        fov = fabsf(fov);
        
        return fov;
    }

    inline float DistanceFOV(const QAngle &viewAngle, const QAngle &aimAngle, float distance) {
        float angleFOV = AngleFOV(viewAngle, aimAngle);
        float extraBehind = 0.0f;
        float distanceFOV = 0.0f;

        if( angleFOV > 90.0f ){ // behind us
            extraBehind = angleFOV - 90.0f; // how much behind us?
            extraBehind = sinf(DEG2RAD(extraBehind)) * distance; // calculate distance factor, we'll add it in the result later.
        }

        angleFOV = std::min(angleFOV, 90.0f); // only get the stuff in front of us
        distanceFOV += sinf(DEG2RAD(angleFOV)) * distance; // stuff in front of us distance factor
        distanceFOV += extraBehind; // stuff behind us distance factor

        return distanceFOV; // from [0.0 * distanc -> 2.0 * distance]; 0.0 = right on target, 2.0 = directly behind.
    }

}
