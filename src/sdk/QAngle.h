#pragma once

#include "Vector.h"
#include "math.h"

enum {
    PITCH = 0,    // up / down
    YAW,        // left / right
    ROLL        // fall over
};

class QAngleByValue;
class QAngle {
public:
    Vector v;

    QAngle()
        : v() {}

    QAngle(const Vector& fv)
        : v(fv.GetAngles(true)) {}

    inline auto operator->() {
        return v.operator->();
    }

    inline auto& operator[](size_t idx) {
        return v[idx];
    }

    inline void Normalize() {
        v.NormalizeAngles<3>(-180.f, 180.f);

        while (v->x > 89.0f)
            v->x -= 180.0f;

        while (v->x < -89.0f)
            v->x += 180.0f;

        v->z = 0;
    }

    inline float Length() const {
        return v.Length();
    }

    inline bool IsValid() const {
        return !isnan(v[0]) && !isnan(v[1]) && !isnan(v[2]);
    }

    inline auto operator+(const QAngle& o) const {
        QAngle ret = *this;
        ret.v += o.v;
        return ret;
    }

    inline auto operator-(const QAngle& o) const {
        QAngle ret = *this;
        ret.v -= o.v;
        return ret;
    }

    inline auto operator*(const QAngle& o) const {
        QAngle ret = *this;
        ret.v *= o.v;
        return ret;
    }

    inline auto operator/(const QAngle& o) const {
        QAngle ret = *this;
        ret.v /= o.v;
        return ret;
    }

    inline auto& operator+=(const QAngle& o) {
        v += o.v;
        return *this;
    }

    inline auto& operator-=(const QAngle& o) {
        v -= o.v;
        return *this;
    }

    inline auto& operator*=(const QAngle& o) {
        v *= o.v;
        return *this;
    }

    inline auto& operator/=(const QAngle& o) {
        v /= o.v;
        return *this;
    }

};
