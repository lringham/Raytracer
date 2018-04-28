#include "Sphere.h"
#include <cmath>

Sphere::Sphere(float radius, Vec3 center, bool invertNormals)
    : radius_(radius), center_(center), invertNormals_(invertNormals)
{

}

bool Sphere::raycast(Ray& ray) const
{
    Vec3 L = center_ - ray.origin_; // Get vector from ray origin to sphere centre
    float tc = L.dot(ray.dir_);	// Project vector onto ray dir

    // Check if ray is pointing at sphere
    if (tc < 0)
        return false;

    // Now solve for d^2
    float d2 = L.dot(L) -  tc*tc;

    // Check if ray missed the sphere
    float radius2 = radius_*radius_;
    if (d2 > radius2)
        return false;

    // Compute distance to p1 from projected ray
    float t1c = std::sqrt(radius2 - d2);

    // Get distance to p1 and p2
    float t1 = tc - t1c;
    float t2 = tc + t1c;

    ray.t_ = t1 >= 0 && t1 < t2 ? t1 : t2;

    if(invertNormals_)
        ray.normal_ = normalize(center_ - ray.intersection());
    else
        ray.normal_ = normalize(ray.intersection() - center_);


    if(dot(Vec3(0, 1, 0), ray.normal_) != 0.f)
        ray.tangent_ = normalize(cross(Vec3(0, 1, 0), ray.normal_));
    else
        ray.tangent_.set(1, 0, 0);

    ray.uv_.set(
                atan2(ray.normal_.x_, ray.normal_.z_) / (3.1415926f*2.f),
                1.f - acos(ray.normal_.y_) / 3.1415926f);

    ray.materialID_ = materialID_;
    return true;
}
