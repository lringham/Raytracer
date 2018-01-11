#pragma once
#include "Geometry.h"

class TriangleMesh : public Geometry
{
  bool intersect(Ray& ray) override;
};
