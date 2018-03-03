#pragma once
#include "Vec3.h"
#include <random>

inline float clamp(float v, float v1, float v2)
{
  return v < v1 ? v1 : v > v2 ? v2 : v;
}

inline Vec3 pointInCircle()
{
	//TODO move creation of this out of function
	std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-1.f, 1.f);
	Vec3 result(0, 0, 0);

	do
	{
		result.set(dis(gen), dis(gen), dis(gen)); // Find point in box around the sphere
	} while(dot(result, result) >= 1.f); // Reject point if outside the sphere

	return result;
}