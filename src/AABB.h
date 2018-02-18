#pragma once
#include "Vec3.h"
#include "Tracable.h"
#include <limits>
#include <algorithm>
#include <vector>

class AABB : public Tracable
{
public:
  AABB();
  AABB(const std::vector<Vec3>& vertices);
  AABB(const Vec3& corner0,const Vec3& corner1);
  bool raycast(Ray& ray) const override;
  void setPosition(const Vec3& position);

private:
  Vec3 _min, _max, _position;
};
