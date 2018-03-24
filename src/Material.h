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

  Material(const std::string& name = "", float Ia = .1f, const Vec3& kd = Vec3(1,1,1), const Vec3& ks = Vec3(1,1,1), const Vec3& attenuation = Vec3(1,1,1), float gloss = 64.f, float eta = 1.f, float reflectivity = 0.f, const std::string& type = "blinnPhong", bool checkered = false);
  void setTexture(const std::string& filename);
  void setNormalMap(const std::string& filename);
  Vec3 color(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& lightColor) const;
  Vec3 sampleTexture(const Vec2& uv);  
  Vec3 attenuationColor(float distance);
  bool isCheckered(const Vec3& point, int width = 5, int height = 5) const;
  float schlick(float eta1, float eta2, float cosTheta) const;

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
    return _texture.isInitialized();
  }

  inline Vec3 sampleTexture(const Vec2& uv) const
  {
    return _texture.sample(uv._u, uv._v);
  }

  inline Vec3 sampleNormalMap(const Vec2& uv) const
  {
    return _normalMap.sample(uv._u, uv._v);
  }

  inline bool hasNormalMap()
  {
    return _hasNormalMap;
  }

  std::string _name;
  float _Ia;
  Vec3 _kd, _ks, _attenuation;
  float _gloss, _eta, _reflectivity;
  MaterialType _type;

private:
  Texture _texture;
  Texture _normalMap;
  bool _hasTexture, _hasNormalMap, _checkered;
};
