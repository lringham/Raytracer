#pragma once
#include "Tracable.h"

class Triangle : public Tracable
{
public:
    Triangle(Vec3 p0 = Vec3(-1, 0, 0), Vec3 p1 = Vec3(0, 1, 0), Vec3 p2 = Vec3(1, 0, 0));
    bool raycast(Ray& ray) const override;

    Vec3 _p0, _p1, _p2;
    Vec3 _t;
    Vec3 _n0, _n1, _n2;
    Vec2 _uv0, _uv1, _uv2;
};

bool raycastTri(const Vec3& p0, const Vec3& p1, const Vec3& p2, Ray& ray, float* uRet = nullptr, float* vRet = nullptr);
Vec3 barycentre(const Triangle& triangle);
Vec3 calculateTangent(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec2& _uv0, const Vec2& _uv1, const Vec2& _uv2);
