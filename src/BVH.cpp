#include "BVH.h"
#include "Triangle.h"
#include <memory>

BVH::BVH()
{

}

BVH::BVH(const Obj& obj)
{
    build(obj);
}

BVH::~BVH()
{
    cleanup(_root._left);
    cleanup(_root._right);

    if(_root._volume != nullptr)
    {
        delete _root._volume;
        _root._volume = nullptr;
    }
}

void BVH::cleanup(Node* node)
{
    if(node == nullptr)
        return;

    cleanup(node->_left);
    cleanup(node->_right);

    if(node->_volume != nullptr)
    {
        delete node->_volume;
        node->_volume = nullptr;
    }
    delete node;
    node = nullptr;
}

void BVH::build(const Obj& obj)
{
    _position = obj._position;
    if(obj._faces.size() == 1)
        _root._volume = new Triangle(
            obj._position + obj._positions[0],
            obj._position + obj._positions[1],
            obj._position + obj._positions[2]);
    else if(obj._faces.size() > 1)
    {
        // Find all triangles
        std::vector<Triangle> triangles(obj._faces.size());
        {
            int i = 0;
            for(auto& f : obj._faces)
                triangles[i++] = Triangle(obj._positions[f.v0],
                                          obj._positions[f.v1],
                                          obj._positions[f.v2]);
        }
        AABB* volume = new AABB(triangles);
        volume->move(_position);

        _root._volume = volume;
        buildChildern(_root, triangles);
    }
}

void BVH::buildChildern(Node& root, const std::vector<Triangle>& triangles, int axis)
{
    if(triangles.size() == 1)
    {
        root._volume = new Triangle(_position + triangles[0]._p0, _position + triangles[0]._p1,_position + triangles[0]._p2);
    }
    else
    {
        axis = (axis + 1) % 3;
        Pair pair = split(triangles, axis);
        
        if(pair._first.size() == 1)
            root._left = new Node(new Triangle(_position + pair._first[0]._p0,_position +  pair._first[0]._p1, _position + pair._first[0]._p2));
        else if(pair._first.size() > 0)
        {
            AABB* volume = new AABB(pair._first);
            volume->move(_position);
            root._left = new Node(volume);
            buildChildern(*root._left, pair._first);
        }

        if(pair._second.size() == 1)
            root._right = new Node(new Triangle(_position + pair._second[0]._p0,_position +  pair._second[0]._p1, _position + pair._second[0]._p2));
        else if(pair._second.size() > 0)
        {
            AABB* volume = new AABB(pair._second);
            volume->move(_position);
            root._right = new Node(volume);
            buildChildern(*root._right, pair._second);
        }
    }
}

BVH::Pair BVH::split(const std::vector<Triangle>& triangles, int axis) const
{
    // std::cout << "Triangles: " << triangles.size();
    Pair pair;
    if(triangles.size() == 0)
        return pair;

    // calculate centre of triangles
    float divider = 0.f;
    // split on x
    if(axis == 0) 
        for(auto& t : triangles)
            divider += barycentre(t)._x;
    // split on y
    else if(axis == 1) 
        for(auto& t : triangles)
            divider += barycentre(t)._y;
    // split on z
    else if(axis == 2)
        for(auto& t : triangles)
            divider += barycentre(t)._z;
    divider /= static_cast<float>(triangles.size());

    // sort triangles
    if(axis == 0) 
    {
        for(auto& t : triangles)
        {
            if(barycentre(t)._x <= divider)
                pair._first.push_back(t);
            else
                pair._second.push_back(t);
        }
    }
    else if(axis == 1) 
    {
        for(auto& t : triangles)
        {
            if(barycentre(t)._y <= divider)
                pair._first.push_back(t);
            else
                pair._second.push_back(t);
        }
    }
    else if(axis == 2)
    {
        for(auto& t : triangles)
        {
            if(barycentre(t)._z <= divider)
                pair._first.push_back(t);
            else
                pair._second.push_back(t);
        }
    }

    // TODO: find better splitting metric
    if(pair._first.size() == 0 && pair._second.size() > 1)
    {
        pair._first.push_back(pair._second.back());
        pair._second.erase(pair._second.end()-1);
    }
    else if(pair._second.size() == 0 && pair._first.size() > 1)
    {
        pair._second.push_back(pair._first.back());
        pair._first.erase(pair._first.end()-1);
    }

    // std::cout << " " << pair._first.size() << " : " << pair._second.size() << std::endl;
    return pair;
}

bool BVH::raycast(Ray& ray) const
{
    return search(&_root, ray);
}

bool BVH::search(const Node* node, Ray& ray) const
{
    Ray parentRay = ray;
    if(node->_volume->raycast(parentRay))
    {        
        if(node->_left == nullptr && node->_right == nullptr)
        {
            ray = parentRay;
            return true;
        }

        Ray leftRay = ray, rightRay = ray;
        bool hitLeft = false, hitRight = false;

        if(node->_left != nullptr)
            hitLeft = search(node->_left, leftRay);
        if(node->_right != nullptr)
            hitRight = search(node->_right, rightRay);
            
        if(hitLeft && hitRight)
        {
            if(leftRay._t < rightRay._t)
                ray = leftRay;
            else
                ray = rightRay;
            return true;
        }
        else if(hitLeft)
        {
            ray = leftRay;
            return true;
        }
        else if(hitRight)
        {
            ray = rightRay;
            return true;
        }
        else
            return false;
    }
    return false;
}