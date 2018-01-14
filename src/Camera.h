#pragma once
#include "Pixels.h"
#include "Ray.h"

class Camera
{
public:
	Camera();
	Ray createRay(unsigned x, unsigned y) const;

	Pixels _pixels;
	Vec3 _position;
};
