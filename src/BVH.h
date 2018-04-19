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
        std::vector<Triangle> _first;
        std::vector<Triangle> _second;
    };

    struct Node
    {
        AABB _boundingBox;
        int _triangleIndex;
        int _rightChildOffset;

        Node(){}

        Node(AABB boundingBox, int triangleIndex = -1, int rightChildOffset = -1) :
            _boundingBox(boundingBox), _triangleIndex(triangleIndex), _rightChildOffset(rightChildOffset)
        {}

        inline bool isLeaf() const
        {
            return _triangleIndex != -1;
        }
    };

    void buildChildern(std::vector<Triangle>& triangles, int parentIndex = 0, int axis = 0);
    BVH::Pair split(const std::vector<Triangle>& triangles, int axis) const;
    bool insert(unsigned destIndex, std::vector<Node>& destTree, unsigned fromIndex, std::vector<Node>& fromTree);
    bool search(Ray& ray, int parentIndex = 0) const;

    std::vector<Node> _tree;
    std::vector<Triangle> _triangles;
    Vec3 _position;
};
