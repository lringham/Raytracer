#include "Material.h"
#include "Utils.h"
#include <math.h>

Material::Material(const std::string& name, float Ia, const Vec3& kd, const Vec3& ks, const Vec3& attenuation, float gloss, float ior, float reflectivity, const std::string& materialType, bool checkered) :
    name_(name), Ia_(Ia), kd_(kd), ks_(ks), attenuation_(attenuation), gloss_(gloss), ior_(ior), reflectivity_(reflectivity), hasTexture_(false), hasNormalMap_(false), hasSpecularMap_(false), checkered_(checkered)
{
    if (materialType == "metallic")
    {
        type_ = MaterialType::metallic;
    }
    else if (materialType == "transparent")
    {
        type_ = MaterialType::transparent;
    }
    else
    {
        type_ = MaterialType::blinnPhong;
    }
}

Vec3 Material::color(const Vec3& N, const Vec3& V, const Vec3& L, const Vec3& lightColor, const Vec2& uv) const
{
    Vec3 color = ambientColor();
    Vec3 H = normalize(L + V);
    Vec3 ks = ks_ * pow(clamp(dot(N, H), 0, 1), gloss_);

    if (hasSpecularMap_)
        ks = ks * specularMap_.sample(uv.u_, uv.v_).length();

    Vec3 kd = kd_ * clamp(dot(N, L), 0, 1);

    if (isMetallic())
    {
        color = color + (1.f - Ia_) * lightColor * (kd + ks);
    }
    else if (isTransparent())
    {
        color = color + (1.f - Ia_) * lightColor * ks;
    }
    else //blinnPhong
    {
        color = color + (1.f - Ia_) * (kd + lightColor * ks);
    }

    return color;
}

Vec3 Material::sampleTexture(const Vec2 & uv)
{
    return texture_.sample(uv.u_, uv.v_);
}

Vec3 Material::attenuationColor(float distance)
{
    return Vec3(
        std::exp(-attenuation_.r_ * distance),
        std::exp(-attenuation_.g_ * distance),
        std::exp(-attenuation_.b_ * distance));
}

void Material::setTexture(const std::string & filename)
{
    hasTexture_ = texture_.loadTexture(filename);
}

void Material::setNormalMap(const std::string & filename)
{
    hasNormalMap_ = normalMap_.loadTexture(filename);
}

void Material::setSpecularMap(const std::string & filename)
{
    hasSpecularMap_ = specularMap_.loadTexture(filename);
}

float Material::schlick(float eta1, float eta2, float cosTheta) const
{
    float R = ((eta1 - eta2) * (eta1 - eta2)) / ((eta1 + eta2) * (eta1 + eta2));
    R = R + (1.f - R) * pow((1.f - cosTheta), 5);
    return (reflectivity_ + (1.f - reflectivity_) * R);
}

bool Material::isCheckered(const Vec3 & point, int width, int height) const
{
    // Checkered plane
    if (checkered_)
    {
        float x = point.x_, y = point.z_;
        int x1 = ((int)x / width) % 2;
        int y1 = ((int)y / height) % 2;
        return (y > 0) ^ y1 ? ((x <= 0 && x1) || (x > 0 && !x1)) : ((x <= 0 && !x1) || (x > 0 && x1));
    }
    else
        return false;
}
