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
    pixels_ = pixels;
    position_ = position;
    direction_ = normalize(direction);

    if (fov == 0)
        fov_ = static_cast<float>(pixels.width()) / static_cast<float>(pixels.height());
    else
        fov_ = fov;

    focalLength_ = focalLength;
    lensRadius_ = lensRadius;
    sampleCount_ = sampleCount;

    float dx = std::tan(fov_ / 2.f) * focalLength;
    float dy = dx * (static_cast<float>(pixels_.height()) / static_cast<float>(pixels_.width()));
    pxWidth_ = (2.f * dx) / pixels_.width();
    pxHeight_ = (2.f * dy) / pixels_.height();

    up_.set(0, 1, 0);
    right_ = normalize(cross(direction_, up_));
    up_ = normalize(cross(right_, direction_));
    topLeft_ = position_ + direction_ * focalLength_ - right_ * dx + up_ * dy;
}


std::vector<Ray> Camera::createRays(unsigned x, unsigned y) const
{
    std::vector<Ray> rays;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.f, 1.f);

    for (int rayID = 0; rayID < sampleCount_; ++rayID)
    {
        Vec3 pos;
        if (lensRadius_ == 0.f)
            pos = position_;
        else
        {
            Vec3 p = pointInCircle(gen) * lensRadius_;
            pos = position_ + right_ * p.x_ + up_ * p.y_;
        }

        Vec3 dir;
        if (sampleCount_ == 1)
        {
            dir = normalize(topLeft_ +
                (x * pxWidth_ + .5f * pxWidth_) * right_ -
                (y * pxHeight_ - .5f * pxHeight_) * up_ - pos);
        }
        else
        {
            dir = normalize(topLeft_ +
                (x * pxWidth_ + dis(gen) * pxWidth_) * right_ -
                (y * pxHeight_ - dis(gen) * pxHeight_) * up_ - pos);
        }
        rays.emplace_back(pos, dir);
    }
    return rays;
}

int Camera::sampleCount() const
{
    return sampleCount_;
}
