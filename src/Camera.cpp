#include "Camera.h"

Camera::Camera()
{
  location.set(100, 0, 0);
}

Ray Camera::createRay(unsigned x, unsigned y) const
{
  return Ray(location, normalize(Vec3(x, pixels.height() - 1 - y, 0) - location));
}
