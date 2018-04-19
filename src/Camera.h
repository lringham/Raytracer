#pragma once
#include <vector>
#include "Pixels.h"
#include "Ray.h"

class Camera
{
public:
    Camera();
    Camera(const Vec3& position, const Vec3& direction, float fov, float focalLength, Pixels pixels, int sampleCount, float lensRadius);
    std::vector<Ray> createRays(unsigned x, unsigned y) const;
    void init(const Vec3& position, const Vec3& direction, float fov, float focalLength, Pixels pixels, int sampleCount, float lensRadius);
    int sampleCount() const;

    Pixels _pixels;
    Vec3 _position;
    Vec3 _direction;

private:
    Vec3 _up;
    Vec3 _right;
    Vec3 _topLeft;

    float _fov;
    float _focalLength;
    float _pxWidth;
    float _pxHeight;
    int _sampleCount;
    float _lensRadius;
};
