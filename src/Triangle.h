#pragma once
#include "Tracable.h"

class Triangle : public Tracable
{
public:
	Triangle(Vec3 p0 = Vec3(-1, 0, 0), Vec3 p1 = Vec3(0, 1, 0), Vec3 p2 = Vec3(1, 0, 0));
	bool raycast(Ray& ray) const override;

private:
	Vec3 _p0, _p1, _p2;
};
