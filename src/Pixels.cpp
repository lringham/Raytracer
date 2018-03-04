#include "Pixels.h"
#include "PPM.h"
#include <math.h>

Pixels::Pixels(unsigned width, unsigned height) :
  _width(width), _height(height), _data(width*height, Vec3(1,1,1))
{}

void Pixels::set(unsigned i, Vec3 color)
{
  _data[i] = color;
}

bool Pixels::save(std::string filename)
{
  std::vector<char> chars(_data.size()*3);
  size_t i = 0;
  for(Vec3& c : _data)
  {
    chars[i]    = static_cast<char>(std::pow(c._x, 1.f/2.2f) * 255);
    chars[i+1]  = static_cast<char>(std::pow(c._y, 1.f/2.2f) * 255);
    chars[i+2]  = static_cast<char>(std::pow(c._z, 1.f/2.2f) * 255);
    i += 3;
  }

  return savePPM(filename, _width, _height, &chars[0]);
}

unsigned Pixels::width() const
{
  return _width;
}

unsigned Pixels::height() const
{
  return _height;
}
