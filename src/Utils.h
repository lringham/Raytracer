#pragma once

inline float clamp(float v, float v1, float v2)
{
  return v < v1 ? v1 : v > v2 ? v2 : v;
}

inline float pow(float v, int n)
{
  float temp = 1.f;
  while(n-- > 0)
    temp *= v;
  return temp;
}
