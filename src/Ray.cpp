#include "Ray.h"
#include <limits>

Ray::Ray(Vec3 origin, Vec3 dir) :
  origin(origin), dir(dir)
{
  t = std::numeric_limits<float>::max();
}
