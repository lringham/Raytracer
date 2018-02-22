#include "BVH.h"
#include "Triangle.h"
#include <memory>
#include <limits>

BVH::BVH()
{

}

BVH::BVH(const Obj& obj)
{
    build(obj);
}

bool BVH::raycast(Ray& ray) const
{
    if(_tree.size() == 0)
        return false;
    
    return search(0, ray);
}

bool BVH::search(int nodeIndex, Ray& ray) const
{
    if(!_tree[nodeIndex].isLeaf())
    {
        Ray leftRay = ray, rightRay = ray;
        bool hitLeft = false, hitRight = false;

        if(_tree[nodeIndex].hasLeft())
            hitLeft = search(leftChild(nodeIndex), leftRay);
        if(_tree[nodeIndex].hasRight())
            hitRight = search(rightChild(nodeIndex), rightRay);

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
    return _triangles[_tree[nodeIndex]._triangleIndex].raycast(ray);
}

//==================
void BVH::build(const Obj& obj)
{
    // Init
    _position = obj._position;
    std::vector<std::vector<Node>> subtrees;
    _triangles.resize(obj._faces.size());
    {
        int i = 0;
        for(auto& f : obj._faces)
        {
            _triangles[i] = Triangle( _position + obj._positions[f.v0],
                                      _position + obj._positions[f.v1],
                                      _position + obj._positions[f.v2]);
            Node node(AABB(_triangles[i]), i);
            subtrees.emplace_back(1, node);            
            i++;
        }
    }

    // build tree    
    while(subtrees.size() > 1)
    {
        // Find the closest root
        std::vector<Node> leftTree = subtrees.back();
  
        int closestIndex = -1;
        float distance = std::numeric_limits<float>::max();
        for(unsigned j = 0; j < subtrees.size() - 1; ++j)
        {
            // find closest 
            float tempDistance = length(leftTree[0]._bounds.center() - subtrees[j][0]._bounds.center());
            if(tempDistance < distance)
            {
                distance = tempDistance;
                closestIndex = j;
            }
        }

        // pair up nodes (and their subtrees)
        if(closestIndex != -1)
        {
            // Add a new subtree and root node
            std::vector<Node> rightTree = subtrees[closestIndex];
            subtrees.pop_back();
            subtrees.erase(subtrees.begin() + closestIndex);

            // 
            Node root(AABB(leftTree[0]._bounds, rightTree[0]._bounds));
            std::vector<Node> newTree(1, root);
            subtrees.insert(subtrees.begin(), newTree);            

            // Add childern to new subtree
            insert(leftChild(0), subtrees[0], 0, leftTree);
            insert(rightChild(0), subtrees[0], 0, rightTree);
        }
    }
    _tree = subtrees[0];

    std::cout << "built BVH\n";
    std::cout << "\t triangles "<< _triangles.size() <<"\n";
    std::cout << "\t nodes "<< _tree.size() <<"\n";
}

bool BVH::insert(unsigned destIndex, std::vector<Node>& destTree, unsigned fromIndex, std::vector<Node>& fromTree)
{
    if(fromIndex >= fromTree.size())
        return false;

    if(destIndex >= destTree.size())
        destTree.resize(destIndex+1);

    // insert
    destTree[destIndex] = fromTree[fromIndex];

    // This check should be sufficient because
    // the tree is constructed by pairing sets of two
    if(!fromTree[fromIndex].isLeaf())
    {
        insert(leftChild(destIndex), destTree, leftChild(fromIndex), fromTree);
        insert(rightChild(destIndex), destTree, rightChild(fromIndex), fromTree);
    }

    return true;
}