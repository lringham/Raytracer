#pragma once
#include "Tracable.h"

class Triangle : public Tracable
{
  bool raycast(Ray& ray) const override;
};
