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

void BVH::build(const Obj& obj)
{
    // Find all triangles        
    _position = obj._position;
    std::vector<Triangle> triangles(obj._faces.size());
    {
        int i = 0;
        for(auto& f : obj._faces)
        {
            triangles[i] = Triangle(obj._positions[f._v0] + _position,
                                    obj._positions[f._v1] + _position,
                                    obj._positions[f._v2] + _position);

            if(obj.hasTextureCoordinates())
            {
                triangles[i]._t0 = obj._textureCoords[f._t0];            
                triangles[i]._t1 = obj._textureCoords[f._t1];
                triangles[i]._t2 = obj._textureCoords[f._t2];
            }

            i++;
        }
    }
    
    // Create root and childern
    _tree.emplace_back(AABB(triangles));
    buildChildern(triangles);      
    _tree.shrink_to_fit();
    _triangles.shrink_to_fit();

    // std::cout << "BVH created" << std::endl;
    // std::cout << "\tleaf count " << _triangles.size() << std::endl;
    // std::cout << "\ttree size " << _tree.size() << std::endl;
}


// create childern in depth first order
void BVH::buildChildern(std::vector<Triangle>& triangles, int parentIndex, int axis)
{      
    if(triangles.size() == 1)
    {
        _triangles.push_back(triangles[0]);
        _tree[parentIndex]._triangleIndex = _triangles.size()-1;
    }
    else if(triangles.size() > 1)
    {
        Pair pair = split(triangles, axis);
        axis = (axis + 1) % 3;

        // Insert left
        _tree.emplace_back(AABB(pair._first));
        buildChildern(pair._first, _tree.size() - 1, axis);

        // Insert right
        _tree.emplace_back(AABB(pair._second));
        _tree[parentIndex]._rightChildOffset = _tree.size() - 1;
        buildChildern(pair._second, _tree.size() - 1, axis);
    }
}

BVH::Pair BVH::split(const std::vector<Triangle>& triangles, int axis) const
{
    Pair pair;
    if(triangles.size() == 0)
        return pair;

    // TODO: find better splitting metric
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

    return pair;
}

bool BVH::raycast(Ray& ray) const
{
    return search(ray);
}


bool BVH::search(Ray& ray, int parentIndex) const
{
    Ray parentRay = ray;
    const Node& node = _tree[parentIndex];
    if(node.isLeaf())
    {
        if(_triangles[node._triangleIndex].raycast(parentRay))
        {
            ray = parentRay;
            return true;
        }
        return false;
    }
    else if(node._boundingBox.raycast(parentRay))
    {        
        Ray leftRay = ray, rightRay = ray;
        bool hitLeft = false, hitRight = false;

        bool hasLeft = !node.isLeaf() && node._rightChildOffset != parentIndex + 1; 
        if(hasLeft) // has left child
            hitLeft = search(leftRay, parentIndex + 1); // search left
        if(node._rightChildOffset != -1) // has right child
            hitRight = search(rightRay, node._rightChildOffset); // search right
            
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