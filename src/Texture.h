#pragma once
#include <string>
#include <vector>
#include "Vec3.h"

class Texture
{
public:
    enum FilterMode {
        NEAREST,
        LINEAR
    } filter = LINEAR;

    Texture();
    Texture(const std::string& filename);
    bool loadTexture(const std::string& filename);

    Vec3 sample(float u, float v) const;
    Vec3 getPixelColor(int x, int y) const;
    bool isInitialized() const;


private:
    std::vector<unsigned char> _pixels;
    int _width, _height, _numComp;
    bool _initalized;
};
