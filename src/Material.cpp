#include "Material.h"

Material::Material(const Vec3& diffuseColor, const Vec3& specularColor, float ambient, float gloss) :
  diffuseColor(diffuseColor), specularColor(specularColor), ambient(ambient), gloss(gloss)
{}

Vec3 Material::blinnPhong(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& lightColor)
{
	Vec3 H = normalize(L + V);

	Vec3 tempColor = lightColor *
    (diffuseColor * clamp(dot(N, L), 0, 1) + // Diffuse color
    pow(clamp(dot(N, H), 0, 1), gloss));     // Specular color

	return diffuseColor*ambient + (1.f-ambient) * tempColor;
}

Vec3 Material::ambientColor() const
{
  return diffuseColor*ambient;
}
