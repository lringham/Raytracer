#pragma once
#include "Tracable.h"
#include "Ray.h"
#include "AABB.h"
#include "Obj.h"
#include <vector>

class BVH : public Tracable
{
public:
    BVH();
    void build(const Obj& obj);
    bool raycast(Ray& ray) const override;

private:
    std::vector<AABB> tree;
};

