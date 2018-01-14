#include "Plane.h"
#include <cassert>

Plane::Plane(Vec3 normal, Vec3 position) :
  _normal(normal), _position(position)
{}

bool Plane::raycast(Ray& ray) const
{
	float rayDirDotNormal = ray._dir.dot(_normal);
	if (rayDirDotNormal == 0)
		return false;
	ray._t = (_position - ray._origin).dot(_normal) / rayDirDotNormal;
  ray._normal = _normal;

	return true;
}
