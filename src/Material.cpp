#include "Material.h"
#include <math.h>

Material::Material(const std::string& name, float Ia, const Vec3& kd, const Vec3& ks, float gloss, float eta, bool reflective, bool transparent) :
  _name(name), _Ia(Ia), _kd(kd), _ks(ks), _gloss(gloss), _eta(eta), _reflective(reflective), _transparent(transparent)
{}

Vec3 Material::blinnPhong(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& lightColor)
{
	Vec3 H = normalize(L + V);
  Vec3 ks = _ks * pow(clamp(dot(N, H), 0, 1), _gloss);
  Vec3 kd = _kd * clamp(dot(N, L), 0, 1);
  return _Ia*_kd + (1.f-_Ia) * lightColor * (kd + ks);

	// Vec3 tempColor = lightColor *
  // (_kd * clamp(dot(N, L), 0, 1) + // Diffuse color
  //    _ks * pow(clamp(dot(N, H), 0, 1), _gloss));     // Specular color
  // if(_transparent)
  //   return Ia*_ka + (1.f-Ia) * ks;
  // else
  // {
  // }
}

Vec3 Material::ambientColor() const
{
  return _Ia*_kd;
}

float Material::schlick(float angle, float eta1, float eta2)
{
  float r0 = pow((eta1-eta2) / (eta1+eta2), 2);
  return r0 + (1.f-r0) * pow((1.f-std::cos(angle)), 5);
}

//https://computergraphics.stackexchange.com/questions/3884/controlling-reflection-and-refraction-with-material-properties-in-ray-tracing/3894
//https://www.brown.edu/research/labs/mittleman/sites/brown.edu.research.labs.mittleman/files/uploads/lecture13_0.pdf
