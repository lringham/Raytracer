#pragma once
#include "Tracable.h"

class Sphere : public Tracable
{
  bool raycast(Ray& ray) const override;
  float radius;
};
