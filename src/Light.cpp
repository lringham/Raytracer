#include "Light.h"
#include "Sphere.h"

Light::Light(Vec3 color, Vec3 position, float radius, float intesity) :
  _color(color), _position(position), _radius(radius), _intensity(intesity)
{}

Ray Light::getShadowRay(const Vec3& collisionPoint, float offset)
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

float Light::attenuation(const Vec3& pos) const
{
  Vec3 D = _position - pos;
  float DdotD = dot(D, D);
  if(DdotD != 0.f)
    return _intensity / DdotD;
  else
    return 1.f;
}
