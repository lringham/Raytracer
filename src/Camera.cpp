#include "Camera.h"
#include <cmath>
#include <random>

Camera::Camera()
{}

Camera::Camera(Vec3 position, float fov, float focalLength, Pixels pixels) :
   _pixels(pixels), _position(position), _fov(fov), _focalLength(focalLength)
{}

void Camera::init(Vec3 position, float fov, float focalLength, Pixels pixels)
{
  _pixels = pixels;
  _position = position;
  _fov = fov;
  _focalLength = focalLength;

  float ratio = static_cast<float>(_pixels.width()) / static_cast<float>(_pixels.height());
  float theta = _fov / 2.f;
  float dx = std::tan(theta) * focalLength;

  _x0 = _position._x - dx;
  _x1 = _position._x + dx;

  float dy = (_x1 - _x0) / (ratio * 2.f);

  _y0 = _position._y + dy;
  _y1 = _position._y - dy;

  _pxWidth  = (_x1 - _x0)  / _pixels.width();
  _pxHeight = (_y0 - _y1) / _pixels.height();
}

std::vector<Ray> Camera::createRays(unsigned x, unsigned y) const
{
  std::vector<Ray> rays;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.f, 1.0001f);

  for(int rayID=0; rayID<10; ++rayID)
  {
    rays.push_back( Ray(_position,
      normalize(
        Vec3(_x0 + x * _pxWidth + dis(gen)*_pxWidth, _y0 - y * _pxHeight + dis(gen)*_pxHeight, -_focalLength)
       - _position)));
  }

  return rays;
}
