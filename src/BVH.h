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
    ~BVH();
    void cleanup(Node* n);

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
        Node* _left = nullptr;
        Node* _right = nullptr;
        Tracable* _volume;

        Node(Tracable* volume) :
            _volume(volume)
        {}

        Node()
        {}
    };

    Pair split(const std::vector<Triangle>& triangles, int axis) const;
    void buildChildern(Node& root, const std::vector<Triangle>& triangles, int level = 0);
    bool search(const Node* node, Ray& ray) const;

    Vec3 _position;
    Node _root;
};
  
// inline int leftChild(int i) const
// {
//     return 2*i+1;
// }

// inline int rightChild(int i) const
// {
//     return 2*i+2;
// }

// inline int parent(int i) const
// {
//     if(i < 3)
//         return 0;
//     else
//         return (i-2)/2 + (i%2);
// }