#include "Ray.h"
#include <limits>

Ray::Ray(Vec3 origin, Vec3 dir, float offset) :
    origin_(origin), dir_(dir)
{
    t_ = std::numeric_limits<float>::max();
    origin_ = origin_ + dir_ * offset;
    ior_ = 1.f;
}

Ray::Ray() :
    origin_(Vec3(0, 0, 0)), dir_(Vec3(0, 0, -1))
{
    t_ = std::numeric_limits<float>::max();
}

Vec3 Ray::intersection() const
{
    return origin_ + dir_ * t_;
}
