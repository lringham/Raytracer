#pragma once
#include "Tracable.h"

class Plane : public Tracable
{
public:
	Plane(Vec3 normal = Vec3(0,1,0), Vec3 position = Vec3(0, 0, 0));

	bool raycast(Ray& ray) const override;

private:
	Vec3 position;
	Vec3 normal;
};
