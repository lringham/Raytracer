#pragma once
#include "Geometry.h"

class Plane : public Geometry
{
  bool intersect(Ray& ray) override;
};
