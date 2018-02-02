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
  float schlick(const Vec3& I, const Vec3& N, float eta1, float eta2);

  std::string _name;
  float _Ia;
  Vec3 _kd, _ks;
  float _gloss, _eta;
  bool _reflective, _transparent;
};
