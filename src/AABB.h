#pragma once
#include "Vec3.h"
#include "Tracable.h"
#include <limits>
#include <algorithm>

class AABB : public Tracable
{
public:
  AABB(const Vec3& corner0,const Vec3& corner1)
  {
    _min.x = std::min(corner0.x, corner1.x);
    _min.y = std::min(corner0.y, corner1.y);
    _min.z = std::min(corner0.z, corner1.z);
    _max.x = std::max(corner0.x, corner1.x);
    _max.y = std::max(corner0.y, corner1.y);
    _max.z = std::max(corner0.z, corner1.z);
  }

  bool trace(Ray& ray) const override
  {
    // float tNear = std::numeric_limits<float>::min();
    // float tFar = std::numeric_limits<float>::max();
    // float x0, x1, y0, y1, z0, z1;
    //
    // // y = mx + b
    // // solving for x
    // // slope (m) is encoded in the ray direction
    // // y is given by slab (plane)
    // if(ray._dir._x != 0.f)
    // {
    //   x0 = (ray._origin.x - _min._x) / ray._dir._x;
    //   x1 = (ray._origin.x - _max._x) / ray._dir._x;
    // }
    //
    // if(ray._dir._y != 0.f)
    // {
    //   y0 = (ray._origin.y - _min._y) / ray._dir._y;
    //   y1 = (ray._origin.y - _max._y) / ray._dir._y;
    // }
    //
    // if(ray._dir._z != 0.f)
    // {
    //   z0 = (ray._origin.z - _min._z) / ray._dir._z;
    //   z1 = (ray._origin.z - _max._z) / ray._dir._z;
    // }
    return false;
  }

private:
  Vec3 _min, _max;
};
