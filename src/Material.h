#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include "Light.h"
#include "Texture.h"
#include <string>
class Material
{
public:
  enum MaterialType {
    blinnPhong,
    transparent,
    metallic
  };

  Material(const std::string& name = "", float Ia = .1f, const Vec3& kd = Vec3(1,1,1), const Vec3& ks = Vec3(1,1,1), float gloss = 64.f, float eta = 1.f, const std::string& type = "blinnPhong");
  void setTexture(const std::string& filename);
  Vec3 color(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& lightColor) const;
  Vec3 sampleTexture(const Vec2& uv);

  inline Vec3 ambientColor() const
  {
    return _Ia*_kd;
  }

  inline bool isBlinnPhong() const
  {
    return _type == blinnPhong;
  };
  inline bool isTransparent() const
  {
    return _type == transparent;
  };
  inline bool isMetallic() const
  {
    return _type == metallic;
  };

  inline bool hasTexture()
  {
    return _hasTexture;
  }

  std::string _name;
  float _Ia;
  Vec3 _kd, _ks;
  float _gloss, _eta;
  MaterialType _type;
  Texture _texture;

private:
  bool _hasTexture;
};

float schlick(float eta1, float eta2, const Vec3& N, const Vec3& I);
