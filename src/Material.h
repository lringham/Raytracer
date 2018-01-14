#pragma once
#include "Vec3.h"

class Material
{
public:
  Material(const Vec3& diffuseColor = Vec3(1, 0, 0), const Vec3& specularColor = Vec3(1, 1, 1), float ambient = .1f, float gloss = 64.f);
  Vec3 blinnPhong(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& lightColor);
  Vec3 ambientColor() const;

  Vec3 _diffuseColor;
  Vec3 _specularColor;
  float _ambient, _gloss;


	inline float clamp(float v, float v1, float v2)
	{
		return v < v1 ? v1 : v > v2 ? v2 : v;
	}

	inline float pow(float v, int n)
	{
		float temp = 1.f;
		while(n-- > 0)
			temp *= v;
		return temp;
	}


};
