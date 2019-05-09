#pragma once
#include "Tracable.h"
#include <limits>
#include "Mat3.h"

class Plane : public Tracable
{
public:
    Plane(Vec3 normal = Vec3(0, 1, 0), Vec3 position = Vec3(0, 0, 0), float width = std::numeric_limits<float>::max(), float height = std::numeric_limits<float>::max());
    bool raycast(Ray& ray) const override;

private:
    Mat3 m;
    Vec3 normal_;
    Vec3 tangent_;
    Vec3 position_;
    float width_, height_, halfWidth_, halfHeight_;
};
