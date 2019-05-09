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
    BVH(const Obj& obj);

    void build(const Obj& obj);
    bool raycast(Ray& ray) const override;

private:
    struct Pair
    {
        std::vector<Triangle> first_;
        std::vector<Triangle> second_;
    };

    struct Node
    {
        AABB boundingBox_;
        int triangleIndex_;
        int rightChildOffset_;

        Node() {}

        Node(AABB boundingBox, int triangleIndex = -1, int rightChildOffset = -1) :
            boundingBox_(boundingBox), triangleIndex_(triangleIndex), rightChildOffset_(rightChildOffset)
        {}

        inline bool isLeaf() const
        {
            return triangleIndex_ != -1;
        }
    };

    void buildChildern(std::vector<Triangle>& triangles, int parentIndex = 0, int axis = 0);
    BVH::Pair split(const std::vector<Triangle>& triangles, int axis) const;
    bool insert(unsigned destIndex, std::vector<Node>& destTree, unsigned fromIndex, std::vector<Node>& fromTree);
    bool search(Ray& ray, int parentIndex = 0) const;

    std::vector<Node> tree_;
    std::vector<Triangle> triangles_;
    Vec3 position_;
};
