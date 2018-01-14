#pragma once
#include "Vec3.h"

class Light
{
public:
  Light(Vec3 color = Vec3(1, 1, 1), Vec3 position = Vec3(0, 0, 0));

  Vec3 color;
  Vec3 position;
};
