#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
{

}

Texture::Texture(const std::string& filename)
{
  if(!loadTexture(filename))
    std::cout << "Cannot load texture:" << filename << "\n";
}

Vec3 Texture::sample(float u, float v)
{
    unsigned x = static_cast<unsigned>(u * (_width  - 1));
    unsigned y = static_cast<unsigned>(v * (_height - 1));
    int i = (x + y*_width) * _numComp;

    return Vec3(
       static_cast<float>(_pixels[i]  ) / 255.f,
       static_cast<float>(_pixels[i+1]) / 255.f,
       static_cast<float>(_pixels[i+2]) / 255.f);

}

bool Texture::loadTexture(const std::string& filename)
{
  //TODO: handle different number of components
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(&filename[0], &_width, &_height, &_numComp, 3);
  if(data != nullptr)
  {
    std::cout << "Texture loaded: " << filename << "\n\twidth: " << _width << "\n\theight: " << _height << "\n\tnum comps: " << _numComp << "\n";
    for(int i = 0; i < _width*_height*3; ++i)
      _pixels.push_back(data[i]);
    _pixels.shrink_to_fit();
    stbi_image_free(data);
    return true;
  }
  return false;
}
