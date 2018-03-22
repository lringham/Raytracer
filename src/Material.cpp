#include "Material.h"
#include "Utils.h"
#include <math.h>

Material::Material(const std::string& name, float Ia, const Vec3& kd, const Vec3& ks, const Vec3& attenuation, float gloss, float eta, float reflectivity, const std::string& materialType, bool checkered) :
  _name(name), _Ia(Ia), _kd(kd), _ks(ks), _attenuation(attenuation), _gloss(gloss), _eta(eta), _reflectivity(reflectivity), _hasTexture(false), _hasNormalMap(false), _checkered(checkered)
{
  if(materialType == "metallic")
  {
    _type = MaterialType::metallic;
  }
  else if(materialType == "transparent")
  {
    _type = MaterialType::transparent;
  }
  else
  {
    _type = MaterialType::blinnPhong;
  }
}

Vec3 Material::color(const Vec3& N, const Vec3& V, const Vec3& L, const Vec3& lightColor) const
{
  Vec3 color = ambientColor();
	Vec3 H = normalize(L + V);
  Vec3 ks = _ks * pow(clamp(dot(N, H), 0, 1), _gloss);
  Vec3 kd = _kd * clamp(dot(N, L), 0, 1);

  if(isMetallic())
  {
    color = color + (1.f-_Ia) * lightColor * (kd + ks);
  }
  else if(isTransparent())
  {
    color = color + (1.f-_Ia) * lightColor * ks;
  }
  else
  {
    color = color + (1.f-_Ia) * (kd + lightColor * ks);
  }

  return color;
}

Vec3 Material::sampleTexture(const Vec2& uv)
{
  return _texture.sample(uv._u, uv._v);
}

Vec3 Material::attenuationColor(float distance)
{  
  return Vec3(
    std::exp(-_attenuation._r * distance), 
    std::exp(-_attenuation._g * distance), 
    std::exp(-_attenuation._b * distance));
}

void Material::setTexture(const std::string& filename)
{
  _hasTexture = _texture.loadTexture(filename);
}

void Material::setNormalMap(const std::string& filename)
{
  _hasNormalMap = _normalMap.loadTexture(filename);
}

float Material::schlick(float eta1, float eta2, float cosTheta) const
{
			float R = ((eta1-eta2)*(eta1-eta2)) / ((eta1+eta2)*(eta1+eta2));
			R = R + (1.f - R) * pow((1.f-cosTheta), 5);	
			return (_reflectivity + (1.0 - _reflectivity) * R);
}

bool Material::isCheckered(const Vec3& point, int width, int height) const
{
    // Checkered plane
    if(_checkered)
    {
      float x = point._x, y = point._z;
      int x1 = ((int)x / width) % 2;
      int y1 = ((int)y / height) % 2;
      //TODO: make this more readable
      return (y > 0) ^ y1 ? ((x <= 0 && x1) || (x > 0 && !x1)) : ((x <= 0 && !x1) || (x > 0 && x1)); 
    }
    else 
      return false;
}