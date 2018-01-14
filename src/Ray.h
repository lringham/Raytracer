#pragma once
#include "Vec3.h"

class Ray
{
public:
  Ray();
  Ray(Vec3 origin, Vec3 dir, float offset = 0.f);
  Vec3 intersection() const;

  Vec3 origin;
  Vec3 dir;
  Vec3 normal;
  float t;
};
