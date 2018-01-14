#include "Camera.h"
#include <cmath>

Camera::Camera()
{}

Camera::Camera(Vec3 position, float fov, float focalLength, Pixels pixels) :
  _position(position), _fov(fov), _focalLength(focalLength), _pixels(pixels)
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

  std::cout << "w: " << _pixels.width() << ", h: " << _pixels.height() << "\n";
  std::cout << "[x0 " << _x0 << ", " << _y0 << ", " << _x1 << ", " << _y1 << "]\n";
  std::cout << "_pxWidth: " << _pxWidth << ", _pxHeight: " << _pxHeight << ", ratio: " << ratio << "\n";
}

std::vector<Ray> Camera::createRays(unsigned x, unsigned y) const
{
  std::vector<Ray> rays;
  rays.push_back(Ray(_position, normalize(Vec3(_x0+x*_pxWidth+_pxWidth/2.f, _y0-y*_pxHeight+_pxHeight/2.f, -_focalLength) - _position)));

  //Anti aliasing
  rays.push_back(Ray(_position, normalize(Vec3(_x0+x*_pxWidth,          _y0-y*_pxHeight+_pxHeight/2.f, -_focalLength) - _position)));
  rays.push_back(Ray(_position, normalize(Vec3(_x0+x*_pxWidth,          _y0-y*_pxHeight-_pxHeight/2.f, -_focalLength) - _position)));
  rays.push_back(Ray(_position, normalize(Vec3(_x0+x*_pxWidth+_pxWidth, _y0-y*_pxHeight-_pxHeight/2.f, -_focalLength) - _position)));
  rays.push_back(Ray(_position, normalize(Vec3(_x0+x*_pxWidth+_pxWidth, _y0-y*_pxHeight+_pxHeight/2.f, -_focalLength) - _position)));
  return rays;
}
