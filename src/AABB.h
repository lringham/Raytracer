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
  AABB(const AABB& b0,const AABB& b1);
  AABB(const Vec3& v0, const Vec3& v1, const Vec3& v2);
  AABB(const std::vector<Vec3>& vertices);
  AABB(const Vec3& corner0,const Vec3& corner1);
  bool raycast(Ray& ray) const override;
  void setPosition(const Vec3& position);
  Vec3 getPosition() const;
  
private:
  Vec3 _min, _max, _position;
};
