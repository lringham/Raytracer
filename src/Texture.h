#pragma once
#include <string>
#include <vector>
#include "Vec3.h"

class Texture
{
public:
    enum class FilterMode
    {
        NEAREST,
        LINEAR
    } filter = FilterMode::LINEAR;

    Texture();
    Texture(const std::string& filename);
    bool loadTexture(const std::string& filename);

    Vec3 sample(float u, float v) const;
    Vec3 getPixelColor(int x, int y) const;
    bool isInitialized() const;


private:
    std::vector<unsigned char> pixels_;
    int width_ = 0, height_ = 0, numComp_ = 0;
    bool initalized_ = false;
};
