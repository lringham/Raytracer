#include "BVH.h"
#include "Triangle.h"
#include <memory>

BVH::BVH()
{

}

void BVH::build(const Obj& obj)
{
    // // Building from top down
    // const unsigned NUM_FACES = obj._faces.size();
    // if(NUM_FACES == 1)
    // {
    //     _tree.emplace_back(
    //         Triangle(obj._positions[obj._faces[0].v0],
    //                  obj._positions[obj._faces[0].v1],
    //                  obj._positions[obj._faces[0].v2]));
    // }
    // else if(NUM_FACES != 0)
    // {
    //     _tree.resize(NUM_FACES * 2 - 1);

    //     // bound triangles
    //     std::vector<std::unique_ptr<Tracable>> nodes;
    //     for(unsigned i = 0; i < NUM_FACES; ++i)
    //     {
    //         nodes.emplace_back(
    //             new Triangle(obj._positions[obj._faces[i].v0],
    //                          obj._positions[obj._faces[i].v1],
    //                          obj._positions[obj._faces[i].v2]));
    //     }

    //     // bound pairs of nodes until there is 1 left
    //     int index = 0;
    //     while(nodes.size() > 0)
    //     {
    //         float distance = std::numeric_limits<float>::max();
    //         for(int i = 0; i < nodes.size(); i++)
    //         {   
    //             // find best pair             
    //             int smallestIndex = -1;
    //             for(int j = i+1; j < nodes.size(); ++j)
    //             {
    //                 float tempDistance = length(nodes[i].getPosition() - nodes[j].getPosition());
    //                 if(distance > tempDistance)
    //                 {
    //                     distance = tempDistance;
    //                     smallestIndex = j;
    //                 }
    //             }

    //             // pair and insert 
    //             if(smallestIndex >= 0)
    //             {
    //                 //TODO account for position?
    //                 AABB left = nodes[i];
    //                 AABB right = nodes[smallestIndex];
    //                 AABB parent = AABB(left, right);

    //                 tempBoxes.emplace_back();   
    //                 // root = new AABB
    //                 // left = boundingBoxes[i]
    //                 // right = boundingBoxes[smallestIndex]
    //             }
    //             else
    //             {
    //                 tempBoxes.emplace_back(boundingBoxes[i]);
    //             }
    //         } 

    //         boundingBoxes = tempBoxes;
    //         tempBoxes.clear();
    //     }
    // }
}

bool BVH::raycast(Ray& ray) const
{
    return false;
}