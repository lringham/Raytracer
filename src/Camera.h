#pragma once
#include <vector>
#include "Pixels.h"
#include "Ray.h"

class Camera
{
public:
	Camera();
	Camera(Vec3 position, float fov, float focalLength, Pixels pixels, int sampleCount);
	std::vector<Ray> createRays(unsigned x, unsigned y) const;
	void init(Vec3 position, float fov, float focalLength, Pixels pixels, int sampleCount);
	int sampleCount() const;

	Pixels _pixels;
	Vec3 _position;

private:
	float _fov;
	float _focalLength;
	float _pxWidth;
	float _pxHeight;
	float _x0, _x1, _y0, _y1;
	int _sampleCount;
};
