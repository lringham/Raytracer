#include "Plane.h"
#include <cmath>

Plane::Plane(Vec3 normal, Vec3 position, float width, float depth) :
  _normal(normal), _position(position), _width(width), _depth(depth)
{
   _halfWidth = _width / 2.f;
   _halfDepth = _depth / 2.f;
  float nDotUp = dot(normal, Vec3(0, 1, 0));
  if (nDotUp*nDotUp != 1.f)
  {
    Vec3 wid = normalize(cross(Vec3(0, 1, 0), normal));
    Vec3 dep = normalize(cross(normal, wid));
    m.setCol(0, wid);
    m.setCol(1, _normal);
    m.setCol(2, dep);
    m.transpose();
  }
  else
    m.identity();
}

bool Plane::raycast(Ray& ray) const
{
	float rayDirDotNormal = ray._dir.dot(_normal);
	if (rayDirDotNormal == 0)
		return false;


  ray._t = (_position - ray._origin).dot(_normal) / rayDirDotNormal;
  ray._normal = _normal;

  Vec3 dist = m * (ray.intersection() - _position);
  if(std::abs(dist._x) < _halfWidth && std::abs(dist._z) < _halfDepth)
  {
    ray._uv.set(
      (dist._x + _halfWidth) / _width,
      (dist._z + _halfDepth) / _depth);

    return true;
  }
	return false;
}
