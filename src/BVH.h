#pragma once
#include "Tracable.h"
#include "Ray.h"
#include "AABB.h"
#include "Obj.h"
#include <vector>

class BVH : public Tracable
{
    struct Node;

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
        AABB _bounds;
        int _triangleIndex;

        Node(){}

        Node(AABB bounds, int triangleIndex = -1) :
            _bounds(bounds), _triangleIndex(triangleIndex)
        {}

        bool isLeaf() const
        {
            return _triangleIndex != -1;
        }

        bool hasLeft() const
        {
            return true;
        }

        bool hasRight() const
        {
            return true;
        }
    };

    inline int leftChild(int i) const
    {
        return 2*i+1;
    }

    inline int rightChild(int i) const
    {
        return 2*i+2;
    }

    inline int parent(int i) const
    {
        if(i < 3)
            return 0;
        else
            return (i-2)/2 + (i%2);
    }

    bool insert(unsigned destIndex, std::vector<Node>& destTree, unsigned fromIndex, std::vector<Node>& fromTree);
    bool search(int nodeIndex, Ray& ray) const;

    std::vector<Node> _tree;
    std::vector<Triangle> _triangles;
    Vec3 _position;
};