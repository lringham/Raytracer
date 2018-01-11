#include "Plane.h"
#include <cassert>

Plane::Plane(Vec3 normal, Vec3 position) :
  normal(normal), position(position)
{}

bool Plane::raycast(Ray& ray) const
{
	float rayDirDotNormal = ray.dir.dot(normal);
	if (rayDirDotNormal == 0)
		return false;
	ray.t = (position - ray.origin).dot(normal) / rayDirDotNormal;
	return true;
}
