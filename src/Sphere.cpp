#include "Sphere.h"
#include <cmath>

Sphere::Sphere(float radius, Vec3 center)
	: radius(radius), center(center)
{

}

bool Sphere::raycast(Ray& ray) const
{
	Vec3 L = center - ray.origin; // Get vector from ray origin to sphere centre
	float tc = L.dot(ray.dir);	// Project vector onto ray dir

	// Check if ray is pointing at sphere
	if (tc < 0)
		return false;

	// Now solve for d^2
	float d2 = L.dot(L) -  tc*tc;

	// Check if ray missed the sphere
	float radius2 = radius*radius;
	if (d2 > radius2)
		return false;

	// Compute distance to p1 from projected ray
	float t1c = std::sqrt(radius2 - d2);

	// Get distance to p1 and p2
	float t1 = tc - t1c;
	float t2 = tc + t1c;

	ray.t = t1 >= 0 && t1 <= t2 ? t1 : t2;
	ray.normal = normalize(ray.intersectioon() - center);
	return true;
}
