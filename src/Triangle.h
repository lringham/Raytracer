#pragma once
#include "Tracable.h"

class Triangle : public Tracable
{
public:
    Triangle(Vec3 p0 = Vec3(-1, 0, 0), Vec3 p1 = Vec3(0, 1, 0), Vec3 p2 = Vec3(1, 0, 0));
    bool raycast(Ray& ray) const override;

    Vec3 p0_, p1_, p2_;
    Vec3 t_;
    Vec3 n0_, n1_, n2_;
    Vec2 uv0_, uv1_, uv2_;
};

bool raycastTri(const Vec3& p0, const Vec3& p1, const Vec3& p2, Ray& ray, float* uRet = nullptr, float* vRet = nullptr);
Vec3 barycentre(const Triangle& triangle);
Vec3 calculateTangent(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec2& uv0_, const Vec2& uv1_, const Vec2& uv2_);
