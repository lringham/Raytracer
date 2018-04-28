#pragma once
#include "Vec3.h"
#include "Vec2.h"

class Ray
{
public:
    Ray();
    Ray(Vec3 origin, Vec3 dir, float offset = 0.f);
    Vec3 intersection() const;

    Vec3 origin_;
    Vec3 dir_;
    Vec3 normal_;
    Vec3 tangent_;
    Vec2 uv_;
    float t_, ior_;
    int materialID_;
};
