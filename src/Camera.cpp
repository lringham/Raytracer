#include "Camera.h"

Camera::Camera()
{
  location.set(400, 400, 1000);
}

Ray Camera::createRay(unsigned x, unsigned y) const
{
  return Ray(location, normalize(Vec3(x, (pixels.height() - 1 - y), -1) - location));
}
