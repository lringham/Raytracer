#include "Material.h"

Material::Material(const Vec3& diffuseColor, const Vec3& specularColor, float ambient, float gloss) :
  _diffuseColor(diffuseColor), _specularColor(specularColor), _ambient(ambient), _gloss(gloss)
{}

Vec3 Material::blinnPhong(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& lightColor)
{
	Vec3 H = normalize(L + V);

	Vec3 tempColor = lightColor *
    (_diffuseColor * clamp(dot(N, L), 0, 1) + // Diffuse color
    pow(clamp(dot(N, H), 0, 1), _gloss));     // Specular color

	return _diffuseColor*_ambient + (1.f-_ambient) * tempColor;
}

Vec3 Material::ambientColor() const
{
  return _diffuseColor*_ambient;
}
