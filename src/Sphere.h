#pragma once
#include "Geometry.h"

class Sphere : public Geometry
{
  bool intersect(Ray& ray) override;
  float radius;
};
