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
    struct Node
    {
        Tracable* left;
        Tracable* right;
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


    std::vector<Tracable> _tree;
};