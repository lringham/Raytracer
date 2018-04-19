#pragma once
#include "Tracable.h"
#include <limits>
#include "Mat3.h"

class Plane : public Tracable
{
public:
    Plane(Vec3 normal = Vec3(0,1,0), Vec3 position = Vec3(0, 0, 0), float width = std::numeric_limits<float>::max(), float height = std::numeric_limits<float>::max());
    bool raycast(Ray& ray) const override;

private:
    Mat3 m;
    Vec3 _normal;
    Vec3 _tangent;
    Vec3 _position;
    float _width, _height, _halfWidth, _halfHeight;
};
