#include "AABB.h"

AABB::AABB() :
    min_(std::numeric_limits<float>::max()), max_(std::numeric_limits<float>::min())
{}

AABB::AABB(const AABB& b0, const AABB& b1)
{
    min_.x_ = std::min(b0.min_.x_, b1.min_.x_);
    min_.y_ = std::min(b0.min_.y_, b1.min_.y_);
    min_.z_ = std::min(b0.min_.z_, b1.min_.z_);

    max_.x_ = std::max(b0.max_.x_, b1.max_.x_);
    max_.y_ = std::max(b0.max_.y_, b1.max_.y_);
    max_.z_ = std::max(b0.max_.z_, b1.max_.z_);
}

AABB::AABB(const std::vector<Vec3>& points)
{
    if (points.size() == 0)
        return;

    min_ = points[0];
    max_ = points[0];
    for (const Vec3& v : points)
    {
        min_.x_ = std::min(min_.x_, v.x_);
        min_.y_ = std::min(min_.y_, v.y_);
        min_.z_ = std::min(min_.z_, v.z_);
        max_.x_ = std::max(max_.x_, v.x_);
        max_.y_ = std::max(max_.y_, v.y_);
        max_.z_ = std::max(max_.z_, v.z_);
    }
}

AABB::AABB(const Triangle& t)
{
    min_.x_ = t.p0_.x_;
    min_.y_ = t.p0_.y_;
    min_.z_ = t.p0_.z_;
    max_.x_ = t.p0_.x_;
    max_.y_ = t.p0_.y_;
    max_.z_ = t.p0_.z_;

    min_.x_ = std::min(min_.x_, t.p1_.x_);
    min_.y_ = std::min(min_.y_, t.p1_.y_);
    min_.z_ = std::min(min_.z_, t.p1_.z_);
    max_.x_ = std::max(max_.x_, t.p1_.x_);
    max_.y_ = std::max(max_.y_, t.p1_.y_);
    max_.z_ = std::max(max_.z_, t.p1_.z_);

    min_.x_ = std::min(min_.x_, t.p2_.x_);
    min_.y_ = std::min(min_.y_, t.p2_.y_);
    min_.z_ = std::min(min_.z_, t.p2_.z_);
    max_.x_ = std::max(max_.x_, t.p2_.x_);
    max_.y_ = std::max(max_.y_, t.p2_.y_);
    max_.z_ = std::max(max_.z_, t.p2_.z_);
}

AABB::AABB(const std::vector<Triangle>& triangles)
{
    if (triangles.size() > 0)
    {
        const Triangle& t = triangles[0];
        min_.x_ = std::min(t.p0_.x_, std::min(t.p1_.x_, t.p2_.x_));
        min_.y_ = std::min(t.p0_.y_, std::min(t.p1_.y_, t.p2_.y_));
        min_.z_ = std::min(t.p0_.z_, std::min(t.p1_.z_, t.p2_.z_));
        max_.x_ = std::max(t.p0_.x_, std::max(t.p1_.x_, t.p2_.x_));
        max_.y_ = std::max(t.p0_.y_, std::max(t.p1_.y_, t.p2_.y_));
        max_.z_ = std::max(t.p0_.z_, std::max(t.p1_.z_, t.p2_.z_));
    }

    for (const auto& t : triangles)
    {
        min_.x_ = std::min(min_.x_, std::min(t.p0_.x_, std::min(t.p1_.x_, t.p2_.x_)));
        min_.y_ = std::min(min_.y_, std::min(t.p0_.y_, std::min(t.p1_.y_, t.p2_.y_)));
        min_.z_ = std::min(min_.z_, std::min(t.p0_.z_, std::min(t.p1_.z_, t.p2_.z_)));

        max_.x_ = std::max(max_.x_, std::max(t.p0_.x_, std::max(t.p1_.x_, t.p2_.x_)));
        max_.y_ = std::max(max_.y_, std::max(t.p0_.y_, std::max(t.p1_.y_, t.p2_.y_)));
        max_.z_ = std::max(max_.z_, std::max(t.p0_.z_, std::max(t.p1_.z_, t.p2_.z_)));
    }
}

AABB::AABB(const Vec3& corner0, const Vec3& corner1)
{
    min_.x_ = std::min(corner0.x_, corner1.x_);
    min_.y_ = std::min(corner0.y_, corner1.y_);
    min_.z_ = std::min(corner0.z_, corner1.z_);
    max_.x_ = std::max(corner0.x_, corner1.x_);
    max_.y_ = std::max(corner0.y_, corner1.y_);
    max_.z_ = std::max(corner0.z_, corner1.z_);
}

bool AABB::raycast(Ray& ray) const
{
    float tMin = std::numeric_limits<float>::min();
    float tMax = std::numeric_limits<float>::max();

    // Solve for distance from each set of planes (slabs)
    // if the min of all the intersections is less than the  max
    // of all the intersections then the box is hit
    if (ray.dir_.x_ != 0.f)
    {
        float tx0 = (min_.x_ - ray.origin_.x_) / ray.dir_.x_;
        float tx1 = (max_.x_ - ray.origin_.x_) / ray.dir_.x_;
        tMin = std::max(tMin, std::min(tx0, tx1));
        tMax = std::min(tMax, std::max(tx0, tx1));
    }

    if (ray.dir_.y_ != 0.f)
    {
        float ty0 = (min_.y_ - ray.origin_.y_) / ray.dir_.y_;
        float ty1 = (max_.y_ - ray.origin_.y_) / ray.dir_.y_;
        tMin = std::max(tMin, std::min(ty0, ty1));
        tMax = std::min(tMax, std::max(ty0, ty1));
    }

    if (ray.dir_.z_ != 0.f)
    {
        float tz0 = (min_.z_ - ray.origin_.z_) / ray.dir_.z_;
        float tz1 = (max_.z_ - ray.origin_.z_) / ray.dir_.z_;
        tMin = std::max(tMin, std::min(tz0, tz1));
        tMax = std::min(tMax, std::max(tz0, tz1));
    }

    if (tMin <= tMax && tMin >= 0)
    {
        ray.normal_.set(0, 0, 1);
        ray.t_ = tMin;
        return true;
    }
    return false;
}

bool AABB::fastRaycast(Ray & ray, const Vec3 & invDir) const
{
    float tx0 = (min_.x_ - ray.origin_.x_) * invDir.x_;
    float tx1 = (max_.x_ - ray.origin_.x_) * invDir.x_;
    float tMin = std::min(tx0, tx1);
    float tMax = std::max(tx0, tx1);

    float ty0 = (min_.y_ - ray.origin_.y_) * invDir.y_;
    float ty1 = (max_.y_ - ray.origin_.y_) * invDir.y_;
    tMin = std::max(tMin, std::min(ty0, ty1));
    tMax = std::min(tMax, std::max(ty0, ty1));

    float tz0 = (min_.z_ - ray.origin_.z_) * invDir.z_;
    float tz1 = (max_.z_ - ray.origin_.z_) * invDir.z_;
    tMin = std::max(tMin, std::min(tz0, tz1));
    tMax = std::min(tMax, std::max(tz0, tz1));

    if (tMin <= tMax && tMin >= 0)
    {
        ray.t_ = tMin;
        return true;
    }
    return false;
}

void AABB::move(Vec3 translation)
{
    min_ = min_ + translation;
    max_ = max_ + translation;
}

Vec3 AABB::center() const
{
    return min_ + (min_ + max_) * .5f;
}
