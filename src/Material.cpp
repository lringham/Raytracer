#include "Material.h"
#include "Utils.h"
#include <math.h>

Material::Material(const std::string& name, float Ia, const Vec3& kd, const Vec3& ks, float gloss, float eta, const std::string& materialType) :
  _name(name), _Ia(Ia), _kd(kd), _ks(ks), _gloss(gloss), _eta(eta)
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

Vec3 Material::color(const Vec3& N,const Vec3& V, const Vec3& L, const Vec3& pos, const Light& light) const
{
  Vec3 color = ambientColor();
	Vec3 H = normalize(L + V);
  Vec3 ks = _ks * pow(clamp(dot(N, H), 0, 1), _gloss);
  Vec3 kd = _kd * clamp(dot(N, L), 0, 1);

  if(isMetallic())
  {
    color = color + (1.f-_Ia) * light._color * light.attenuation(pos) * (kd + ks);
  }
  else if(isTransparent())
  {
    color = color + (1.f-_Ia) * light._color * light.attenuation(pos) * ks;
  }
  else
  {
    color = color + (1.f-_Ia) * light._color * light.attenuation(pos) * (kd + ks);
  }

  return color;
}

float Material::schlick(const Vec3& I, const Vec3& N, float eta1, float eta2) const
{
  float r0 = pow((eta1-eta2) / (eta1+eta2), 2);
  return r0 + (1.f-r0) * pow((1.f-dot(I, N)), 5);
}

//float FresnelReflectAmount (float n1, float n2, vec3 normal, vec3 incident)
//{
//        // Schlick aproximation
//        float r0 = (n1-n2) / (n1+n2);
//        r0 *= r0;
//        float cosX = -dot(normal, incident);
//        if (n1 > n2)
//        {
//            float n = n1/n2;
//            float sinT2 = n*n*(1.0-cosX*cosX);
//            // Total internal reflection
//            if (sinT2 > 1.0)
//                return 1.0;
//            cosX = sqrt(1.0-sinT2);
//        }
//        float x = 1.0-cosX;
//        float ret = r0+(1.0-r0)*x*x*x*x*x;
//
//        // adjust reflect multiplier for object reflectivity
//        ret = (OBJECT_REFLECTIVITY + (1.0-OBJECT_REFLECTIVITY) * ret);
//        return ret;
//}
