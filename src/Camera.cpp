#include "Camera.h"
#include <cmath>
#include <random>

Camera::Camera()
{}

Camera::Camera(const Vec3& position, const Vec3& direction, float fov, float focalLength, Pixels pixels, int sampleCount, float lensRadius)
{
  init(position, direction, fov, focalLength, pixels, sampleCount, lensRadius);
}

void Camera::init(const Vec3& position, const Vec3& direction, float fov, float focalLength, Pixels pixels, int sampleCount, float lensRadius)
{
  _pixels = pixels;
  _position = position;
  _direction = normalize(direction);
  _fov = fov;
  _focalLength = focalLength;
  _lensRadius = lensRadius;
  _sampleCount = sampleCount;

  float dx = std::tan(_fov / 2.f) * focalLength;
  float dy = dx * (static_cast<float>(_pixels.height()) / static_cast<float>(_pixels.width()));
  _pxWidth  = (2.f * dx)  / _pixels.width();
  _pxHeight = (2.f * dy) / _pixels.height();
  
  _up.set(0, 1, 0);
  _right = normalize(cross(_direction, _up));
  _up    = normalize(cross(_right, _direction));
  _topLeft = _position + _direction * _focalLength - _right * dx + _up * dy;
}


std::vector<Ray> Camera::createRays(unsigned x, unsigned y) const
{
  std::vector<Ray> rays;
  std::random_device rd;
  std::mt19937 gen(1); 

  for(int rayID = 0; rayID < _sampleCount; ++rayID)
  {

    Vec3 pos;
    if(_lensRadius == 0.f)
      pos = _position;
    else
    {           
      std::uniform_real_distribution<> lengthDist(0.f, _lensRadius);
      std::uniform_real_distribution<> angleDist(0.f, 6.28318530718f);

      float ang = angleDist(gen);
      float len = lengthDist(gen);

      pos = _position + _right*cos(ang)*len + _up*sin(ang)*len;
    }

    std::uniform_real_distribution<> dis(0.f, 1.f);

    Vec3 dir = normalize(_topLeft + 
        x*_right*_pxWidth + dis(gen)*_right*_pxWidth -
        y*_pxHeight*_up - dis(gen)*_pxHeight*_up 
        - pos);

    rays.emplace_back(pos, dir);
  }

  return rays;
}

int Camera::sampleCount() const
{
  return _sampleCount;
}
