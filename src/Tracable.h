#pragma once
#include "Ray.h"

class Tracable
{
public:
  Tracable();
  virtual bool raycast(Ray& ray) const;
};
