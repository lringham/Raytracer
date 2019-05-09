#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() :
    initalized_(false)
{}

Texture::Texture(const std::string& filename)
{
    if (!loadTexture(filename))
        std::cout << "Cannot load texture:" << filename << "\n";
}

Vec3 Texture::getPixelColor(int x, int y) const
{
    if (x >= width_)
        x = x % width_;

    if (y >= height_)
        y = y % height_;

    int i = (x + y * width_) * numComp_;

    if (numComp_ == 1)
        return Vec3(static_cast<float>(pixels_[i]) / 255.f, 0, 0);
    else if (numComp_ == 2)
        return Vec3(
            static_cast<float>(pixels_[i]) / 255.f,
            static_cast<float>(pixels_[i + 1]) / 255.f, 0);
    else
        return Vec3(
            static_cast<float>(pixels_[i]) / 255.f,
            static_cast<float>(pixels_[i + 1]) / 255.f,
            static_cast<float>(pixels_[i + 2]) / 255.f);
}

Vec3 Texture::sample(float u, float v) const
{
    if (filter == NEAREST)
    {
        return getPixelColor(
            static_cast<unsigned>(u * width_),
            static_cast<unsigned>(v * height_));
    }
    else
    {
        int x = static_cast<unsigned>(u * width_);
        int y = static_cast<unsigned>(v * height_);

        Vec3 px0 = getPixelColor(x, y);
        Vec3 px1 = getPixelColor(x, y + 1);
        Vec3 px2 = getPixelColor(x + 1, y + 1);
        Vec3 px3 = getPixelColor(x + 1, y);

        float du = 1.f / width_;
        float dv = 1.f / height_;
        float u0 = static_cast<float>(x) / width_;
        float v0 = static_cast<float>(y) / height_;

        float s = (u - u0) / du;
        float t = (v - v0) / dv;

        return lerp(t, lerp(s, px0, px3), lerp(s, px1, px2));
    }
}

bool Texture::loadTexture(const std::string & filename)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(&filename[0], &width_, &height_, &numComp_, 0);
    if (data != nullptr)
    {
        for (int i = 0; i < width_ * height_ * numComp_; ++i)
            pixels_.push_back(data[i]);
        pixels_.shrink_to_fit();
        stbi_image_free(data);
        initalized_ = true;
    }
    return initalized_;
}

bool Texture::isInitialized() const
{
    return initalized_;
}
