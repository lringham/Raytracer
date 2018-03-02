#include "Material.h"
#include "Utils.h"
#include <math.h>

Material::Material(const std::string& name, float Ia, const Vec3& kd, const Vec3& ks, float gloss, float eta, const std::string& materialType, bool checkered) :
  _name(name), _Ia(Ia), _kd(kd), _ks(ks), _gloss(gloss), _eta(eta), _hasTexture(false), _checkered(checkered)
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
    color = color + (1.f-_Ia) * lightColor * (kd + ks);
  }

  return color;
}

Vec3 Material::sampleTexture(const Vec2& uv)
{
  return _texture.sample(uv._u, uv._v);
}

void Material::setTexture(const std::string& filename)
{
  _hasTexture = _texture.loadTexture(filename);
}

float schlick(float eta1, float eta2, const Vec3& N, const Vec3& I)
{
        // Schlick aproximation
        float r0 = (eta1-eta2) / (eta1+eta2);
        r0 *= r0;
        float cosX = -dot(N, I);
        if (eta1 > eta2)
        {
            float eta = eta1/eta2;
            float sinT2 = eta*eta*(1.0-cosX*cosX);
            // Total internal reflection
            if (sinT2 > 1.0)
                return 1.0;
            cosX = sqrt(1.0-sinT2);
        }
        float x = 1.0-cosX;
        float ret = r0+(1.0-r0)*x*x*x*x*x;

        //ret = (OBJECT_REFLECTIVITY + (1.0-OBJECT_REFLECTIVITY) * ret);
        return ret;
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