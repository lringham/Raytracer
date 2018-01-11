#pragma once
#include "Tracable.h"

class Plane : public Tracable
{
  bool raycast(Ray& ray) const override;
};
