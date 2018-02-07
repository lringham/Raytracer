#include "Camera.h"
#include <cmath>
#include <random>

Camera::Camera()
{}

Camera::Camera(Vec3 position, float fov, float focalLength, Pixels pixels, int sampleCount, float lensRadius) :
   _pixels(pixels), _position(position), _fov(fov), _focalLength(focalLength), _sampleCount(sampleCount), _lensRadius(lensRadius)
{}

void Camera::init(Vec3 position, float fov, float focalLength, Pixels pixels, int sampleCount, float lensRadius)
{
  _pixels = pixels;
  _position = position;
  _fov = fov;
  _focalLength = focalLength;
  _lensRadius = lensRadius;

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

  _sampleCount = sampleCount;
}

std::vector<Ray> Camera::createRays(unsigned x, unsigned y) const
{
  std::vector<Ray> rays;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.f, 1.f);

  for(int rayID = 0; rayID < _sampleCount; ++rayID)
  {

    Vec3 pos;
    if(_lensRadius == 0.f)
      pos = _position;
    else
    {
      Vec3 offset(0, 0, 0);
      std::uniform_real_distribution<> dis(-1.f, 1.f);
      do
      {
      	offset.set(dis(gen), dis(gen), 0);
      } while(dot(offset, offset) >= 1.f);
      pos = _position + offset * _lensRadius;
    }

    rays.push_back( Ray(pos,
     normalize(
       Vec3(_x0 + x * _pxWidth + dis(gen)*_pxWidth, _y0 - y * _pxHeight + dis(gen)*_pxHeight, _position._z - _focalLength)
      - pos)));

    // rays.push_back( Ray(pos,
    //   normalize(
    //     Vec3(_x0 + x * _pxWidth + .5f * _pxWidth, _y0 - y * _pxHeight + .5f * _pxHeight, _position._z - _focalLength)
    //    - pos)));
  }

  return rays;
}

int Camera::sampleCount() const
{
  return _sampleCount;
}
