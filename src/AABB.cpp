#include "AABB.h"

AABB::AABB() :
  _position(0, 0, 0)
{}

AABB::AABB(const std::vector<Vec3>& vertices) :
  _position(0, 0, 0)
{
	  _min = Vec3(std::numeric_limits<float>::max());
		_max = Vec3(std::numeric_limits<float>::min());

		for(const Vec3& v : vertices)
		{
			_min._x = std::min(_min._x, v._x);
			_min._y = std::min(_min._y, v._y);
			_min._z = std::min(_min._z, v._z);

			_max._x = std::max(_max._x, v._x);
			_max._y = std::max(_max._y, v._y);
			_max._z = std::max(_max._z, v._z);
		}
}

AABB::AABB(const Vec3& corner0,const Vec3& corner1) :
  _position(0, 0, 0)
{
  _min._x = std::min(corner0._x, corner1._x);
  _min._y = std::min(corner0._y, corner1._y);
  _min._z = std::min(corner0._z, corner1._z);
  _max._x = std::max(corner0._x, corner1._x);
  _max._y = std::max(corner0._y, corner1._y);
  _max._z = std::max(corner0._z, corner1._z);
}

bool AABB::raycast(Ray& ray) const
{
  float tMin = std::numeric_limits<float>::min();
  float tMax = std::numeric_limits<float>::max();

  // Solve for distance from each set of planes (slabs)
  // if the min of all the intersections is less than the  max
  // of all the intersections then the box is hit
  if(ray._dir._x != 0.f)
  {
    float tx0 = (_min._x - ray._origin._x) / ray._dir._x;
    float tx1 = (_max._x - ray._origin._x) / ray._dir._x;
    tMin = std::max(tMin, std::min(tx0, tx1));
    tMax = std::min(tMax, std::max(tx0, tx1));
  }

  if(ray._dir._y != 0.f)
  {
    float ty0 = (_min._y - ray._origin._y) / ray._dir._y;
    float ty1 = (_max._y - ray._origin._y) / ray._dir._y;
    tMin = std::max(tMin, std::min(ty0, ty1));
    tMax = std::min(tMax, std::max(ty0, ty1));
  }

  if(ray._dir._z != 0.f)
  {
    float tz0 = (_min._z - ray._origin._z) / ray._dir._z;
    float tz1 = (_max._z - ray._origin._z) / ray._dir._z;
    tMin = std::max(tMin, std::min(tz0, tz1));
    tMax = std::min(tMax, std::max(tz0, tz1));
  }

  ray._normal.set(0,0,1);
  ray._t = tMin;
  return tMin <= tMax && tMin >= 0;
}

void AABB::setPosition(const Vec3& position)
{
  _min = _min - _position + position;
  _max = _max - _position + position;
  _position = position;
}