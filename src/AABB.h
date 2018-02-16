#pragma once
#include "Vec3.h"
#include "Tracable.h"
#include <limits>
#include <algorithm>

class AABB : public Tracable
{
public:
  AABB();
  AABB(const Vec3& corner0,const Vec3& corner1);
  bool raycast(Ray& ray) const override;

private:
  Vec3 _min, _max;
};
