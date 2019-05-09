#include "BVH.h"
#include "Triangle.h"
#include <memory>
#include <iostream>

BVH::BVH() {}

BVH::BVH(const Obj& obj)
{
    build(obj);
}

void BVH::build(const Obj& obj)
{
    // Find all triangles
    position_ = obj.position_;
    std::vector<Triangle> triangles;
    {
        int i = 0;
        for (auto& f : obj.faces_)
        {
            triangles.emplace_back(obj.positions_[f.v0_] + position_,
                obj.positions_[f.v1_] + position_,
                obj.positions_[f.v2_] + position_);

            if (obj.hasTextureCoordinates())
            {
                triangles.back().uv0_ = obj.textureCoords_[f.t0_];
                triangles.back().uv1_ = obj.textureCoords_[f.t1_];
                triangles.back().uv2_ = obj.textureCoords_[f.t2_];

                triangles.back().t_ = calculateTangent(
                    triangles.back().p0_,
                    triangles.back().p1_,
                    triangles.back().p2_,
                    triangles.back().uv0_,
                    triangles.back().uv1_,
                    triangles.back().uv2_
                );
            }

            if (obj.hasNormals())
            {
                triangles.back().n0_ = obj.normals_[f.n0_];
                triangles.back().n1_ = obj.normals_[f.n1_];
                triangles.back().n2_ = obj.normals_[f.n2_];
            }

            if (obj.faceMaterialMap_.size() > 0)
                triangles.back().materialID_ = obj.faceMaterialMap_.at(i);
            else
                std::cout << "Mesh has no material\n";

            i++;
        }
        triangles.shrink_to_fit();
    }

    // Create root and childern
    tree_.emplace_back(AABB(triangles));
    buildChildern(triangles);
    tree_.shrink_to_fit();
    triangles_.shrink_to_fit();
}


// create childern in depth first order
void BVH::buildChildern(std::vector<Triangle>& triangles, int parentIndex, int axis)
{
    if (triangles.size() == 1)
    {
        triangles_.push_back(triangles[0]);
        tree_[parentIndex].triangleIndex_ = triangles_.size() - 1;
    }
    else if (triangles.size() > 1)
    {
        Pair pair = split(triangles, axis);
        axis = (axis + 1) % 3;

        // Insert left
        tree_.emplace_back(AABB(pair.first_));
        buildChildern(pair.first_, tree_.size() - 1, axis);

        // Insert right
        tree_.emplace_back(AABB(pair.second_));
        tree_[parentIndex].rightChildOffset_ = tree_.size() - 1;
        buildChildern(pair.second_, tree_.size() - 1, axis);
    }
}

BVH::Pair BVH::split(const std::vector<Triangle> & triangles, int axis) const
{
    Pair pair;
    if (triangles.size() == 0)
        return pair;

    // TODO: find better splitting metric
    // calculate centre of triangles
    float divider = 0.f;
    // split on x
    if (axis == 0)
        for (auto& t : triangles)
            divider += barycentre(t).x_;
    // split on y
    else if (axis == 1)
        for (auto& t : triangles)
            divider += barycentre(t).y_;
    // split on z
    else if (axis == 2)
        for (auto& t : triangles)
            divider += barycentre(t).z_;
    divider /= static_cast<float>(triangles.size());

    // sort triangles
    if (axis == 0)
    {
        for (auto& t : triangles)
        {
            if (barycentre(t).x_ <= divider)
                pair.first_.push_back(t);
            else
                pair.second_.push_back(t);
        }
    }
    else if (axis == 1)
    {
        for (auto& t : triangles)
        {
            if (barycentre(t).y_ <= divider)
                pair.first_.push_back(t);
            else
                pair.second_.push_back(t);
        }
    }
    else if (axis == 2)
    {
        for (auto& t : triangles)
        {
            if (barycentre(t).z_ <= divider)
                pair.first_.push_back(t);
            else
                pair.second_.push_back(t);
        }
    }

    if (pair.first_.size() == 0 && pair.second_.size() > 1)
    {
        pair.first_.push_back(pair.second_.back());
        pair.second_.erase(pair.second_.end() - 1);
    }
    else if (pair.second_.size() == 0 && pair.first_.size() > 1)
    {
        pair.second_.push_back(pair.first_.back());
        pair.first_.erase(pair.first_.end() - 1);
    }

    return pair;
}

bool BVH::raycast(Ray & ray) const
{
    return search(ray);
}


bool BVH::search(Ray & ray, int parentIndex) const
{
    Ray parentRay = ray;
    const Node& node = tree_[parentIndex];
    if (node.isLeaf())
    {
        if (triangles_[node.triangleIndex_].raycast(parentRay))
        {
            ray = parentRay;
            return true;
        }
        return false;
    }
    else if (node.boundingBox_.raycast(parentRay))
    {
        Ray leftRay = ray, rightRay = ray;
        bool hitLeft = false, hitRight = false;

        bool hasLeft = !node.isLeaf() && node.rightChildOffset_ != parentIndex + 1;
        if (hasLeft) // has left child
            hitLeft = search(leftRay, parentIndex + 1); // search left
        if (node.rightChildOffset_ != -1) // has right child
            hitRight = search(rightRay, node.rightChildOffset_); // search right

        if (hitLeft && hitRight)
        {
            if (leftRay.t_ < rightRay.t_)
                ray = leftRay;
            else
                ray = rightRay;
            return true;
        }
        else if (hitLeft)
        {
            ray = leftRay;
            return true;
        }
        else if (hitRight)
        {
            ray = rightRay;
            return true;
        }
        else
            return false;
    }
    return false;
}
