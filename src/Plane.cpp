#include "Plane.h"
#include <cmath>

Plane::Plane(Vec3 normal, Vec3 position, float width, float height) :
  _normal(normal), _position(position), _width(width), _height(height)
{
   _halfWidth = _width / 2.f;
   _halfHeight = _height / 2.f;
  float nDotUp = dot(normal, Vec3(0, 1, 0));
  if (nDotUp*nDotUp != 1.f)
  {
    Vec3 w = normalize(cross(Vec3(0, 1, 0), normal));
    Vec3 h = normalize(cross(normal, w));
    m.setCol(0, w);
    m.setCol(1, _normal);
    m.setCol(2, h);
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
  if(std::abs(dist._x) < _halfWidth && std::abs(dist._z) < _halfHeight)
  {
    ray._uv.set(
      (dist._x + _halfWidth) / _width,
      (dist._z + _halfHeight) / _height);

    return true;
  }
	return false;
}
