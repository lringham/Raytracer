#include "Camera.h"
#include "Utils.h"
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

    if(fov == 0)
        _fov = pixels.width() / pixels.height();
    else
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
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.f, 1.f);

    for(int rayID = 0; rayID < _sampleCount; ++rayID)
    {
        Vec3 pos;
        if(_lensRadius == 0.f)
            pos = _position;
        else
        {
            Vec3 p = pointInCircle(gen) * _lensRadius;
            pos = _position + _right*p._x + _up*p._y;
        }

        Vec3 dir;
        if(_sampleCount == 1)
        {
            dir = normalize(_topLeft +
                            (x*_pxWidth  + .5f*_pxWidth) * _right -
                            (y*_pxHeight - .5f*_pxHeight)* _up    - pos);
        }
        else
        {
            dir = normalize(_topLeft +
                            (x*_pxWidth  + dis(gen)*_pxWidth) * _right -
                            (y*_pxHeight - dis(gen)*_pxHeight)* _up    - pos);
        }
        rays.emplace_back(pos, dir);
    }
    return rays;
}

int Camera::sampleCount() const
{
    return _sampleCount;
}
