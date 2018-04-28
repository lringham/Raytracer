#include "Light.h"
#include "Utils.h"
#include <math.h>

// -----------------
// Base
// -----------------
Light::Light(Vec3 color, float intesity) :
    color_(color), intensity_(intesity)
{}

// -----------------
// Pointlight
// -----------------
PointLight::PointLight(Vec3 color, Vec3 position, float intensity, float radius) :
    Light(color, intensity), position_(position), radius_(radius)
{}

Ray PointLight::getShadowRay(const Vec3& collisionPoint, float offset) const
{  
    Vec3 L = normalize(position_ - collisionPoint);
    if(radius_ == 0)
        return Ray(collisionPoint, L, offset);
    else
    {
        Vec3 shadowDir = normalize((position_ - radius_*(L*.5f + pointInCircle())) - collisionPoint);
        if(dot(shadowDir, L) < 0)
            shadowDir = -1 * shadowDir;
        return Ray(collisionPoint, shadowDir, offset);
    }
}

Vec3 PointLight::getColor(const Vec3& collisionPoint) const
{
    return attenuation(collisionPoint) * color_;
}

Vec3 PointLight::vectorFrom(const Vec3& collisionPoint) const
{
    return position_ - collisionPoint;
}

float PointLight::attenuation(const Vec3& collisionPoint) const
{
    Vec3 D = position_ - collisionPoint;
    float DdotD = dot(D, D);
    if(DdotD != 0.f)
        return intensity_ / DdotD;
    else
        return 1.f;
}

// -----------------
// DirectionalLight
// -----------------
DirectionalLight::DirectionalLight(Vec3 color, float intensity, const Vec3& direction) :
    Light(color, intensity), direction_(normalize(direction))
{}

Ray DirectionalLight::getShadowRay(const Vec3& collisionPoint, float offset) const
{
    return Ray(collisionPoint, -1.f * direction_, offset);
}

Vec3 DirectionalLight::getColor(const Vec3&) const
{
    return color_ * intensity_;
}

Vec3 DirectionalLight::vectorFrom(const Vec3&) const
{
    return -1.f * direction_;
}

// -----------------
// Spotlight
// -----------------
SpotLight::SpotLight(Vec3 color, Vec3 position, float intensity, const Vec3& direction, float cosThetaP, float cosThetaU, float exp) :
    Light(color, intensity), position_(position), direction_(normalize(direction)), cosThetaP_(cosThetaP), cosThetaU_(cosThetaU), exp_(exp)
{}

Ray SpotLight::getShadowRay(const Vec3& collisionPoint, float offset) const
{
    return Ray(collisionPoint, normalize(position_ - collisionPoint), offset);
}

Vec3 SpotLight::getColor(const Vec3& collisionPoint) const
{
    Vec3 L = collisionPoint - position_;
    float dist = L.length();
    L = L / dist;
    float cosThetaS = dot(L, direction_);

    // In the centre of spotlight
    if(cosThetaS >= cosThetaP_)
    {
        return color_;
    }
    // In the penumbra
    else if(cosThetaS > cosThetaU_)
    {
        float denom = cosThetaP_ - cosThetaU_;
        if(denom != 0.f)
        {
            return color_ * std::pow((cosThetaS  - cosThetaU_) / denom, exp_);
        }
        else
            return color_;
    }
    // In umbra
    else
        return Vec3(0);
}

Vec3 SpotLight::vectorFrom(const Vec3& collisionPoint) const
{
    return position_ - collisionPoint;
}
