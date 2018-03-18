#pragma once
#include "Vec3.h"
#include <random>

inline float clamp(float v, float v1, float v2)
{
  return v < v1 ? v1 : v > v2 ? v2 : v;
}

inline Vec3 pointInCircle()
{
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

inline Vec3 pointInCircle(std::mt19937& gen)
{
  std::uniform_real_distribution<> dis(-1.f, 1.f);
	Vec3 result(0, 0, 0);

	do
	{
		result.set(dis(gen), dis(gen), dis(gen)); // Find point in box around the sphere
	} while(dot(result, result) >= 1.f); // Reject point if outside the sphere

	return result;
}

inline bool startsWith(const std::string& string, const std::string& start)
{	
    size_t smallest = std::min(string.size(), start.size());
    for(size_t i = 0; i < smallest; ++i)
        if(start[i] != string[i])
            return false;
    return true;
}