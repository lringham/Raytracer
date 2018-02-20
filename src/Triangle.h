#pragma once
#include "Tracable.h"

class Triangle : public Tracable
{
public:
	Triangle(Vec3 p0 = Vec3(-1, 0, 0), Vec3 p1 = Vec3(0, 1, 0), Vec3 p2 = Vec3(1, 0, 0));
	bool raycast(Ray& ray) const override;

	Vec3 _p0, _p1, _p2;
};

bool raycastTri(const Vec3& p0, const Vec3& p1, const Vec3& p2, Ray& ray, float* uRet = nullptr, float* vRet = nullptr);
Vec3 barcycentre(const Triangle& triangle);