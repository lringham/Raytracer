#pragma once
#include "Vec3.h"
#include "Ray.h"

class Light
{
public:
  Light(Vec3 color = Vec3(1, 1, 1), Vec3 position = Vec3(0, 0, 0), float radius = 0.f, float intensity = 1000.f);
  Ray getShadowRay(const Vec3& pos, float offset);
  float attenuation(const Vec3& pos) const;

  Vec3 _color;
  Vec3 _position;
  float _radius;
  float _intensity;
};
