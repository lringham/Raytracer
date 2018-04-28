#pragma once
#include "Tracable.h"
#include <math.h>

class Sphere : public Tracable
{
public:
    Sphere(float radius = 1.f, Vec3 center = Vec3(0, 0, 0), bool invertNormals = false);

    /* Create a triangle using the ray origin, sphere origin, and the vector from the ray origin to the centre
       of the sphere projected onto the ray direction.

                   Ray Origin
                       /|
                    L / | tc (L projected onto raydir)
       Sphere Centre /__|
                      d

      Create a triangle using p1, sphere origin, and the vector from the ray origin to the centre
      of the sphere projected onto the ray direction.

                     P1
                     /|
                  h / | t1c (|tc| - |p1-)
     Sphere Centre /__|
                    d
    */
    bool raycast(Ray& ray) const override;

private:
    float radius_;
    Vec3 center_;
    bool invertNormals_;
};

inline Vec2 calculateUV(const Vec3& vec) 
{
    return Vec2(
                atan2(vec.x_, vec.z_) / (3.1415926f*2.f),
                1.f - acos(vec.y_) / 3.1415926f);
}
