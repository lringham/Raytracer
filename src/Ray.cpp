#include "Ray.h"
#include <limits>

Ray::Ray(Vec3 origin, Vec3 dir, float offset) :
    _origin(origin), _dir(dir)
{
    _t = std::numeric_limits<float>::max();
    _origin = _origin + _dir * offset;
    _ior = 1.f;
}

Ray::Ray() :
    _origin(Vec3(0, 0, 0)), _dir(Vec3(0, 0, -1))
{
    _t = std::numeric_limits<float>::max();
}

Vec3 Ray::intersection() const
{
    return _origin + _dir * _t;
}
