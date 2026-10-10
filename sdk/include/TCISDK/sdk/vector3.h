#pragma once

#include <TCISDK/internal.h>
#include <cmath>

using namespace TCISDK_Internal;

namespace TCISDK {

class Vector3 {
public:
    float x, y, z;

    inline Vector3() : x(0.f), y(0.f), z(0.f) {}
    inline Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    inline Vector3(const Vector3f& raw) : x(raw.x), y(raw.y), z(raw.z) {}

    inline Vector3f ToRaw() const noexcept { return Vector3f{ x, y, z }; }

    inline float Length() const noexcept { return std::sqrt(x * x + y * y + z * z); }

    inline void Normalize() noexcept {
        float len = Length();
        if (len > 0.0f) {
            x /= len; y /= len; z /= len;
        }
    }
};

}