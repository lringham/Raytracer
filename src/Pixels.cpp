#include "Pixels.h"
#include "PPM.h"

Pixels::Pixels(unsigned width, unsigned height) :
  pxWidth(width), pxHeight(height), data(width*height, Vec3(1,1,1))
{}

void Pixels::set(unsigned i, Vec3 color)
{
  data[i] = color;
}

bool Pixels::save(std::string filename)
{
  std::vector<char> chars(data.size()*3);
  size_t i = 0;
  for(Vec3& c : data)
  {
    chars[i]    = static_cast<char>(c.x * 255);
    chars[i+1]  = static_cast<char>(c.y * 255);
    chars[i+2]  = static_cast<char>(c.z * 255);
    i += 3;
  }

  return savePPM(filename, pxWidth, pxHeight, &chars[0]);
}

unsigned Pixels::width() const
{
  return pxWidth;
}

unsigned Pixels::height() const
{
  return pxHeight;
}
