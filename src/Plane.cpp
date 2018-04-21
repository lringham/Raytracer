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
        _tangent = normalize(cross(Vec3(0, 1, 0), normal));
        Vec3 h = normalize(cross(normal, _tangent));
        m.setCol(0, _tangent);
        m.setCol(1, _normal);
        m.setCol(2, h);
        m.transpose();
    }
    else
    {
        _tangent.set(1,0,0);
        m.identity();
    }
}

bool Plane::raycast(Ray& ray) const
{
    float rayDirDotNormal = ray._dir.dot(_normal);
    if (rayDirDotNormal == 0)
        return false;


    ray._t = (_position - ray._origin).dot(_normal) / rayDirDotNormal;

    Vec3 dist = m * (ray.intersection() - _position);
    if(std::abs(dist._x) < _halfWidth && std::abs(dist._z) < _halfHeight)
    {
        ray._normal = _normal;
        ray._tangent = _tangent;

        ray._uv.set(
                    (dist._x + _halfWidth) / _width,
                    (dist._z + _halfHeight) / _height);

        ray._materialID = _materialID;
        return true;
    }
    return false;
}
