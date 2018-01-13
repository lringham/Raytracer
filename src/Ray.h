#pragma once
#include "Vec3.h"

class Ray
{
public:
  Ray(Vec3 origin = Vec3(0,0,0), Vec3 dir = Vec3(0,0,-1));
  Vec3 intersection() const;

  Vec3 origin;
  Vec3 dir;
  Vec3 normal;
  float t;
};
