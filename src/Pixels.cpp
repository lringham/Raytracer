#include "Pixels.h"
#include "PPM.h"
#include <cmath>

Pixels::Pixels() :
    data_(width_ * height_, Vec3(1, 1, 1))
{}

Pixels::Pixels(unsigned width, unsigned height) :
    width_(width), height_(height), data_(width * height, Vec3(1, 1, 1))
{}

void Pixels::set(unsigned i, Vec3 color)
{
    data_[i] = color;
}

bool Pixels::save(std::string filename)
{
    std::vector<char> chars(data_.size() * 3);
    size_t i = 0;
    for (Vec3& c : data_)
    {
        // Perform gamma correction
        chars[i] = static_cast<char>(std::pow(c.x_, 1.f / 2.2f) * 255);
        chars[i + 1] = static_cast<char>(std::pow(c.y_, 1.f / 2.2f) * 255);
        chars[i + 2] = static_cast<char>(std::pow(c.z_, 1.f / 2.2f) * 255);
        i += 3;
    }

    return savePPM(filename, width_, height_, &chars[0]);
}

unsigned Pixels::width() const
{
    return width_;
}

unsigned Pixels::height() const
{
    return height_;
}
