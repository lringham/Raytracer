#include "Material.h"

Material::Material(const Vec3& kd, const Vec3& ks, float ka, float gloss) :
  _kd(kd), _ks(ks), _ka(ka), _gloss(gloss)
{}

Vec3 Material::blinnPhong(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& lightColor)
{
	Vec3 H = normalize(L + V);

  Vec3 kd = lightColor * _kd * clamp(dot(N, L), 0, 1);
  Vec3 ks = lightColor * _ks * pow(clamp(dot(N, H), 0, 1), _gloss);

	// Vec3 tempColor = lightColor *
  // (_kd * clamp(dot(N, L), 0, 1) + // Diffuse color
  //    _ks * pow(clamp(dot(N, H), 0, 1), _gloss));     // Specular color

	return _kd*_ka + (1.f-_ka) * (kd + ks);
}

Vec3 Material::ambientColor() const
{
  return _kd*_ka;
}
