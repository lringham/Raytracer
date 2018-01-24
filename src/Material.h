#pragma once
#include "Vec3.h"
#include <string>

class Material
{
public:
  Material(const std::string& name = "", float Ia = .1f, const Vec3& kd = Vec3(1,1,1), const Vec3& ks = Vec3(1,1,1), float gloss = 64.f, float eta = 1.f, bool reflective = false, bool transparent = false);
  Vec3 blinnPhong(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& lightColor);
  Vec3 ambientColor() const;
  // Returns the probability of reflection
  float schlick(float angle, float eta1, float eta2);

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

  std::string _name;
  float _Ia;
  Vec3 _kd, _ks;
  float _gloss, _eta;
  bool _reflective, _transparent;
};
