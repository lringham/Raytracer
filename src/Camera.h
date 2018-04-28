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
    Vec3 position_;
    Vec3 direction_;

private:
    Vec3 up_;
    Vec3 right_;
    Vec3 topLeft_;

    float fov_;
    float focalLength_;
    float pxWidth_;
    float pxHeight_;
    int sampleCount_;
    float lensRadius_;
};
