#pragma once
#include "Vec3.h"
#include "Vec2.h"

class Ray
{
public:
  Ray();
  Ray(Vec3 origin, Vec3 dir, float offset = 0.f);
  Vec3 intersection() const;

  Vec3 _origin;
  Vec3 _dir;
  Vec3 _normal;
  Vec2 _uv;
  float _t, _ior;
  int _materialID;
};
