#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include "Light.h"
#include "Texture.h"
#include <string>

class Material
{
public:
    enum MaterialType
    {
        blinnPhong,
        transparent,
        metallic
    };

    Material(const std::string& name = "", float Ia = .1f, const Vec3& kd = Vec3(1, 1, 1), const Vec3& ks = Vec3(1, 1, 1), const Vec3& attenuation = Vec3(1, 1, 1), float gloss = 64.f, float ior = 1.f, float reflectivity = 0.f, const std::string& type = "blinnPhong", bool checkered = false);
    void setTexture(const std::string& filename);
    void setNormalMap(const std::string& filename);
    void setSpecularMap(const std::string& filename);
    Vec3 color(const Vec3& N, const Vec3& V, const Vec3& L, const Vec3& lightColor, const Vec2& uv) const;
    Vec3 sampleTexture(const Vec2& uv);
    Vec3 attenuationColor(float distance);
    bool isCheckered(const Vec3& point, int width = 5, int height = 5) const;
    float schlick(float eta1, float eta2, float cosTheta) const;

    inline Vec3 ambientColor() const
    {
        return Ia_ * kd_;
    }

    inline bool isBlinnPhong() const
    {
        return type_ == blinnPhong;
    };
    inline bool isTransparent() const
    {
        return type_ == transparent;
    };
    inline bool isMetallic() const
    {
        return type_ == metallic;
    };

    inline bool hasTexture()
    {
        return texture_.isInitialized();
    }

    inline Vec3 sampleTexture(const Vec2& uv) const
    {
        return texture_.sample(uv.u_, uv.v_);
    }

    inline Vec3 sampleNormalMap(const Vec2& uv) const
    {
        return normalMap_.sample(uv.u_, uv.v_);
    }

    inline bool hasNormalMap()
    {
        return hasNormalMap_;
    }

    inline bool hasSpecularMap()
    {
        return hasSpecularMap_;
    }

    std::string name_;
    float Ia_ = 0.1f;
    Vec3 kd_ = Vec3(1.f, 1.f, 1.f), ks_ = Vec3(1.f, 1.f, 1.f), attenuation_ = Vec3(1.f, 1.f, 1.f);
    float gloss_ = 1.f, ior_ = 1.f, reflectivity_ = 0.f;
    MaterialType type_;

private:
    Texture texture_;
    Texture normalMap_;
    Texture specularMap_;
    bool hasTexture_, hasNormalMap_, hasSpecularMap_, checkered_;
};
