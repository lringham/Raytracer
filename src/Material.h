#pragma once
class Material
{
public:
  Material(float ks = 1.f, float kc = 1.f, float shininess = 1.f);
  float ks, kc, shininess;
};
