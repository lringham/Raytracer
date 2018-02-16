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
    float tNear = std::numeric_limits<float>::min();
    float tFar = std::numeric_limits<float>::max();

    return tNear <= tFar;
  }

private:
  Vec3 _min, _max;
};
