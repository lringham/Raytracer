#include "Plane.h"
#include <cmath>

Plane::Plane(Vec3 normal, Vec3 position, float width, float height) :
    normal_(normal), position_(position), width_(width), height_(height)
{
    halfWidth_ = width_ / 2.f;
    halfHeight_ = height_ / 2.f;
    float nDotUp = dot(normal, Vec3(0, 1, 0));
    if (nDotUp * nDotUp != 1.f)
    {
        tangent_ = normalize(cross(Vec3(0, 1, 0), normal));
        Vec3 h = normalize(cross(normal, tangent_));
        m.setCol(0, tangent_);
        m.setCol(1, normal_);
        m.setCol(2, h);
        m.transpose();
    }
    else
    {
        tangent_.set(1, 0, 0);
        m.identity();
    }
}

bool Plane::raycast(Ray& ray) const
{
    float rayDirDotNormal = ray.dir_.dot(normal_);
    if (rayDirDotNormal == 0)
        return false;


    ray.t_ = (position_ - ray.origin_).dot(normal_) / rayDirDotNormal;

    Vec3 dist = m * (ray.intersection() - position_);
    if (std::abs(dist.x_) < halfWidth_ && std::abs(dist.z_) < halfHeight_)
    {
        ray.normal_ = normal_;
        ray.tangent_ = tangent_;

        ray.uv_.set(
            (dist.x_ + halfWidth_) / width_,
            (dist.z_ + halfHeight_) / height_);

        ray.materialID_ = materialID_;
        return true;
    }
    return false;
}
