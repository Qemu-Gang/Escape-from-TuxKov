#include "../sdk/Vector.h"
#include "../sdk/QAngle.h"
#include "../utils/Logger.h"
#include "../sdk/CInput.h"
#include "../sdk/Definitions.h"

namespace Math {

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

    inline float DistanceFOV(const QAngle &viewAngle, const QAngle &aimAngle, const float distance) {
        float angleFOV = AngleFOV(viewAngle, aimAngle);

        angleFOV /= 90.0f;

        float distanceFOV = angleFOV * distance;

        return distanceFOV;
    }

    inline void CorrectMovement(CUserCmd* cmd, QAngle oldAngles, float oldForward, float oldSide) {
        float deltaAngles;
        float f1;
        float f2;

        if (oldAngles->y < 0.f)
            f1 = 360.0f + oldAngles->y;
        else
            f1 = oldAngles->y;

        if (cmd->m_viewAngles->y < 0.0f)
            f2 = 360.0f + cmd->m_viewAngles->y;
        else
            f2 = cmd->m_viewAngles->y;

        if (f2 < f1)
            deltaAngles = abs(f2 - f1);
        else
            deltaAngles = 360.0f - abs(f1 - f2);

        deltaAngles = 360.0f - deltaAngles;

        cmd->m_forwardmove = cos(DEG2RAD(deltaAngles)) * oldForward + cos(DEG2RAD(deltaAngles + 90.f)) * oldSide;
        cmd->m_sidemove = sin(DEG2RAD(deltaAngles)) * oldForward + sin(DEG2RAD(deltaAngles + 90.f)) * oldSide;        
    }
}
