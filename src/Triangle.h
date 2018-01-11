#pragma once
#include "Geometry.h"

class Triangle : public Geometry
{
  bool intersect(Ray& ray) override;
};
