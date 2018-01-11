#pragma once
#include "Tracable.h"

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
	float radius;
	Vec3 center;
};
