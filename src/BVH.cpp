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
    //     Triangle t( obj._positions[obj._faces[0].v0], 
    //                 obj._positions[obj._faces[0].v1], 
    //                 obj._positions[obj._faces[0].v2]);

    //     _tree.push_back(new Node(t, barcycentre(t)));
    // }
    // else if(NUM_FACES != 0)
    // {
    //     // bound triangles
    //     std::vector<Node*> nodes;
    //     for(unsigned i = 0; i < NUM_FACES; ++i)
    //     {
    //         Triangle t(obj._positions[obj._faces[i].v0], 
    //                    obj._positions[obj._faces[i].v1], 
    //                    obj._positions[obj._faces[i].v2]);
                        
    //         nodes.push_back(new Node(t, barcycentre(t)));
    //     }

    //     // bound pairs of nodes until there is 1 left
    //     int index = 0;
    //     while(nodes.size() > 0)
    //     {
    //         std::vector<Node*> tempNodes;
    //         float distance = std::numeric_limits<float>::max();
    //         for(int i = 0; i < nodes.size(); i++)
    //         {   
    //             // find best pair      
    //             Node* node = nodes[i];       
    //             int smallestIndex = -1;
    //             for(int j = i+1; j < nodes.size(); ++j)
    //             {
    //                 float tempDistance = length(node->_position - nodes[j]->_position);
    //                 if(distance > tempDistance)
    //                 {
    //                     distance = tempDistance;
    //                     smallestIndex = j;
    //                 }
    //             }

    //             // pair and insert 
    //             if(smallestIndex >= 0)
    //             {
    //                 Node* right = nodes[smallestIndex];                    
    //                 Node* parent = new Node(
    //                     AABB(node->_t, right->_t),
    //                     (node->_position + right->_position) / 2.f);
    //                 parent->_left = node;
    //                 parent->_right = right;
    //                 tempNodes.push_back(parent);
    //             }
    //             else
    //                 tempNodes.push_back(nodes[i]);
    //         } 

    //         nodes = tempNodes;
    //         tempNodes.clear();
    //     }
    // }
}

bool BVH::raycast(Ray& ray) const
{
    return false;
}