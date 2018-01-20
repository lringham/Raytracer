#pragma once
#include "Vec3.h"

class Material
{
public:
  Material(const Vec3& kd = Vec3(1,1,1), const Vec3& ks = Vec3(1,1,1), float ka = .1f, float gloss = 64.f);
  Vec3 blinnPhong(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& lightColor);
  Vec3 ambientColor() const;

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

  Vec3 _kd;
  Vec3 _ks;
  float _ka, _gloss, _ior;
  bool _transparent, _reflective;
};
