#include "Light.h"
#include "Sphere.h"
#include <math.h>

// -----------------
// Base
// -----------------
Light::Light(Vec3 color, float intesity) :
  _color(color), _intensity(intesity)
{}

// -----------------
// Pointlight
// -----------------
PointLight::PointLight(Vec3 color, Vec3 position, float intensity, float radius) :
  Light(color, intensity), _position(position), _radius(radius)
{}

Ray PointLight::getShadowRay(const Vec3& collisionPoint, float offset) const
{
  Vec3 L = normalize(_position - collisionPoint);
  if(_radius == 0)
    return Ray(collisionPoint, L, offset);
  else
  {
    Vec3 shadowDir = normalize((_position - _radius*(L*.5f + randomPosInSphere())) - collisionPoint);
    if(dot(shadowDir, L) < 0)
        shadowDir = -1 * shadowDir;
    return Ray(collisionPoint, shadowDir, offset);
  }
}

Vec3 PointLight::getColor(const Vec3& collisionPoint) const
{
  return attenuation(collisionPoint) * _color;
}

Vec3 PointLight::vectorFrom(const Vec3& collisionPoint) const
{
  return _position - collisionPoint;
}

float PointLight::attenuation(const Vec3& collisionPoint) const
{
  Vec3 D = _position - collisionPoint;
  float DdotD = dot(D, D);
  if(DdotD != 0.f)
    return _intensity / DdotD;
  else
    return 1.f;
}

// -----------------
// DirectionalLight
// -----------------
DirectionalLight::DirectionalLight(Vec3 color, float intensity, const Vec3& direction) :
  Light(color, intensity), _direction(normalize(direction))
{}

Ray DirectionalLight::getShadowRay(const Vec3& collisionPoint, float offset) const
{
  return Ray(collisionPoint, -1.f * _direction, offset);
}

Vec3 DirectionalLight::getColor(const Vec3&) const
{
  return _color;
}

Vec3 DirectionalLight::vectorFrom(const Vec3&) const
{
  return -1.f * _direction;
}

// -----------------
// Spotlight
// -----------------
SpotLight::SpotLight(Vec3 position, Vec3 color, float intensity, const Vec3& direction, float cosThetaP, float cosThetaU, float exp) :
  Light(color, intensity), _position(position), _direction(direction), _cosThetaP(cosThetaP), _cosThetaU(cosThetaU), _exp(exp)
{}

Ray SpotLight::getShadowRay(const Vec3& collisionPoint, float offset) const
{
  return Ray(collisionPoint, normalize(_position - collisionPoint), offset);
}

Vec3 SpotLight::getColor(const Vec3& collisionPoint) const
{
  float cosThetaS = dot(collisionPoint, _direction);

  // In the centre of spotlight
  if(cosThetaS >= _cosThetaP)		
  {
    return _color;
  }
  // In the penumbra
  else if(cosThetaS < _cosThetaP && cosThetaS > _cosThetaU)
  {
    float denom = _cosThetaP - _cosThetaU;
    if(denom > 0.f)
      return _color * std::pow((cosThetaS  - _cosThetaU) / denom, _exp);
    else
      return _color;
  }
  // In umbra
  else
    return Vec3(0);
}

Vec3 SpotLight::vectorFrom(const Vec3& collisionPoint) const
{
  return _position - collisionPoint;
}