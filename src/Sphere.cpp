#include "Sphere.h"
#include <cmath>

Sphere::Sphere(float radius, Vec3 center)
	: _radius(radius), _center(center)
{

}

bool Sphere::raycast(Ray& ray) const
{
	Vec3 L = _center - ray._origin; // Get vector from ray origin to sphere centre
	float tc = L.dot(ray._dir);	// Project vector onto ray dir

	// Check if ray is pointing at sphere
	if (tc < 0)
		return false;

	// Now solve for d^2
	float d2 = L.dot(L) -  tc*tc;

	// Check if ray missed the sphere
	float radius2 = _radius*_radius;
	if (d2 > radius2)
		return false;

	// Compute distance to p1 from projected ray
	float t1c = std::sqrt(radius2 - d2);

	// Get distance to p1 and p2
	float t1 = tc - t1c;
	float t2 = tc + t1c;

	ray._t = t1 >= 0 && t1 < t2 ? t1 : t2;
	ray._normal = normalize(ray.intersection() - _center);
	ray._uv.set(
		atan2(ray._normal._x, ray._normal._z) / (3.1415926f*2.f),
		1.f - acos(ray._normal._y) / 3.1415926f);

	return true;
}
