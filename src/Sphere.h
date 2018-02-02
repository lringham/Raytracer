#pragma once
#include "Tracable.h"
#include <random>

class Sphere : public Tracable
{
public:
	Sphere(float radius = 1.f, Vec3 center = Vec3(0, 0, 0));

	/* Create a triangle using the ray origin, sphere origin, and the vector from the ray origin to the centre
	   of the sphere projected onto the ray direction.

	               Ray Origin
	                   /|
	                L / | tc (L projected onto raydir)
	   Sphere Centre /__|
	                  d

	  Create a triangle using p1, sphere origin, and the vector from the ray origin to the centre
	  of the sphere projected onto the ray direction.

		 			   P1
                     /|
                  h / | t1c (|tc| - |p1-)
	   Sphere Centre /__|
					  d
	*/
	bool raycast(Ray& ray) const override;

private:
	float _radius;
	Vec3 _center;
};

inline Vec3 randomPosInSphere()
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
