#pragma once
#include <vector>
#include <string>
#include "Vec3.h"

class Pixels
{
public:
    Pixels();
    Pixels(unsigned width, unsigned height);
    void set(unsigned i, Vec3 color);
    bool save(std::string filename);
    unsigned width() const;
    unsigned height() const;

private:
    unsigned width_ = 100, height_ = 100;
    std::vector<Vec3> data_;
};
