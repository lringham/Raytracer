#pragma once
#include <vector>
#include <string>
#include "Vec3.h"

class Pixels
{
public:
    Pixels(unsigned width = 0, unsigned height = 0);
    void set(unsigned i, Vec3 color);
    bool save(std::string filename);
    unsigned width() const;
    unsigned height() const;

private:
    unsigned width_ = 0, height_ = 0;
    std::vector<Vec3> data_;
};
