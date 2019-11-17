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

    Pixels pixels_;
    Vec3 position_ = Vec3(0, 0, 1);
    Vec3 direction_ = Vec3(0, 0, -1);

private:
    Vec3 up_ = Vec3(0.f, 1.f, 0.f);
    Vec3 right_ = Vec3(1.f, 0.f, 0.f);
    Vec3 topLeft_ = Vec3(-4.37042f, 4.37042f, -8.f);

    float fov_ = 1.f;
    float focalLength_ = 1.f;
    float lensRadius_ = 0.f;
    float pxWidth_ = 0.0145681f;
    float pxHeight_ = 0.0145681f;
    unsigned sampleCount_ = 1;
};