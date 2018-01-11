#pragma once
#include <vector>
#include "Vec3.h"
#include "Vec2.h"
#include "Transform.h"
#include "Ray.h"
#include "Material.h"

class Geometry
{
public:
  Geometry();
  virtual bool intersect(Ray& ray);

protected:
  std::vector<Vec3> vertices;
  std::vector<Vec3> normals;
  std::vector<Vec2> textureCoordinates;
  Material material;
  Transform transform;
};
