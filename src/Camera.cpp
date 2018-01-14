#include "Camera.h"

Camera::Camera() :
  _position(400, 400, 1000)
{}

Ray Camera::createRay(unsigned x, unsigned y) const
{
  return Ray(_position, normalize(Vec3(x, (_pixels.height() - 1 - y), -1) - _position));
}
