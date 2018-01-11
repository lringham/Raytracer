#pragma once

class Tracable
{
public:
  Tracable();
  virtual bool raycast(Ray& ray);
};
