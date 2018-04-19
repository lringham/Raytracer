#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() : 
    _initalized(false)
{}

Texture::Texture(const std::string& filename)
{
    if(!loadTexture(filename))
        std::cout << "Cannot load texture:" << filename << "\n";
}

Vec3 Texture::getPixelColor(int x, int y) const
{
    if(x >= _width)
        x = x % _width;

    if(y >= _height)
        y = y % _height;

    int i = (x + y*_width) * _numComp;
    if(_numComp == 1)
        return Vec3(static_cast<float>(_pixels[i]  ) / 255.f, 0, 0);
    else if(_numComp == 2)
        return Vec3(
                    static_cast<float>(_pixels[i]  ) / 255.f,
                    static_cast<float>(_pixels[i+1]) / 255.f, 0);
    else
        return Vec3(
                    static_cast<float>(_pixels[i]  ) / 255.f,
                    static_cast<float>(_pixels[i+1]) / 255.f,
                static_cast<float>(_pixels[i+2]) / 255.f);
}

Vec3 Texture::sample(float u, float v) const
{
    if(filter == NEAREST)
    {
        return getPixelColor(
                    static_cast<unsigned>(u*_width),
                    static_cast<unsigned>(v*_height));
    }
    else
    {
        int x = static_cast<unsigned>(u*_width);
        int y = static_cast<unsigned>(v*_height);

        Vec3 px0 = getPixelColor(x,   y);
        Vec3 px1 = getPixelColor(x,   y+1);
        Vec3 px2 = getPixelColor(x+1, y+1);
        Vec3 px3 = getPixelColor(x+1, y);

        float du = 1.f / _width;
        float dv = 1.f / _height;
        float u0 = static_cast<float>(x) / _width;
        float v0 = static_cast<float>(y) / _height;

        float s = (u - u0) / du;
        float t = (v - v0) / dv;
        
        return lerp(t, lerp(s, px0, px3), lerp(s, px1, px2));
    }
}

bool Texture::loadTexture(const std::string& filename)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(&filename[0], &_width, &_height, &_numComp, 0);
    if(data != nullptr)
    {
        for(int i = 0; i < _width*_height*_numComp; ++i)
            _pixels.push_back(data[i]);
        _pixels.shrink_to_fit();
        stbi_image_free(data);
        _initalized = true;
    }
    return _initalized;
}

bool Texture::isInitialized() const
{
    return _initalized;
}
