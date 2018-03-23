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

Vec3 Texture::sample(float u, float v) const
{
    unsigned x = static_cast<unsigned>(u * (_width  - 1));
    unsigned y = static_cast<unsigned>(v * (_height - 1));
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
        return true;
    }
    return false;
}

bool Texture::isInitialized() const
{
    return _initalized;
}