#pragma once
#include <cmath>

namespace GameplayMath
{
    constexpr float Pi = 3.14159265358979323846f;
    inline float Clamp(float v, float lo, float hi)
    {
        return v < lo ? lo : (v > hi ? hi : v);
    }
    inline float WrapAngle(float angle)
    {
        return std::remainder(angle, 2.0f * Pi);
    }
    inline float FollowRate(float speed, float dt)
    {
        return 1.0f - std::exp(-speed * dt);
    }
    inline float FollowAngle(float from, float to, float speed, float dt)
    {
        return WrapAngle(from + WrapAngle(to - from) * FollowRate(speed, dt));
    }
    inline bool SphereHit(float dx, float dy, float dz, float radius)
    {
        return dx * dx + dy * dy + dz * dz <= radius * radius;
    }
}
