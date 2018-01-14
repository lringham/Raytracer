#include "Ray.h"
#include <limits>

Ray::Ray(Vec3 origin, Vec3 dir, float offset) :
  origin(origin), dir(dir)
{
  t = std::numeric_limits<float>::max();
  origin = origin + dir * offset;
}

Ray::Ray() :
  origin(Vec3(0, 0, 0)), dir(Vec3(0, 0, -1))
{
  t = std::numeric_limits<float>::max();
}

Vec3 Ray::intersection() const
{
  return origin + dir * t;
}
