#pragma once
#include <string>
#include <vector>
#include "Vec3.h"

class Texture
{
public:
  enum SampleMode {
    NEAREST,
    LINEAR
  };

  Texture();
  Texture(const std::string& filename);
  Vec3 sample(float u, float v);
  bool loadTexture(const std::string& filename);

private:
  std::vector<unsigned char> _pixels;
  int _width, _height, _numComp;
};
