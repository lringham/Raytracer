#pragma once
#include "Vec3.h"
#include "Tracable.h"
#include "Triangle.h"
#include <limits>
#include <algorithm>
#include <vector>

class AABB : public Tracable
{
public:
    AABB();
    AABB(const AABB& b0,const AABB& b1);
    AABB(const std::vector<Vec3>& points);
    AABB(const Triangle& t);
    AABB(const std::vector<Triangle>& triangles);
    AABB(const Vec3& corner0,const Vec3& corner1);
    bool raycast(Ray& ray) const override;
    bool fastRaycast(Ray& ray,const Vec3& invDir) const;
    void move(Vec3 translation);
    Vec3 center() const;

    Vec3 _min, _max;
};
