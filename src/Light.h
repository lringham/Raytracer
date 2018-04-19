#pragma once
#include "Vec3.h"
#include "Ray.h"
#include <random>

// -----------------
// Base
// -----------------
class Light
{
public:
    Light(Vec3 color = Vec3(1, 1, 1), float intensity = 1000.f);
    virtual Ray getShadowRay(const Vec3& collisionPoint, float offset) const = 0;
    virtual Vec3 getColor(const Vec3& collisionPoint) const = 0;
    virtual Vec3 vectorFrom(const Vec3& collisionPoint) const = 0;

protected:
    Vec3 _color;
    float _intensity;
};

// -----------------
// Pointlight
// -----------------
class PointLight : public Light
{
public:
    PointLight(Vec3 color = Vec3(1, 1, 1), Vec3 position = Vec3(0, 0, 0), float intensity = 1000.f, float radius = 0.f);
    Ray getShadowRay(const Vec3& collisionPoint, float offset) const override;
    Vec3 getColor(const Vec3& collisionPoint) const override;
    Vec3 vectorFrom(const Vec3& collisionPoint) const override;

private:
    float attenuation(const Vec3& collisionPoint) const;

    Vec3 _position;
    float _radius;
};

// -----------------
// DirectionalLight
// -----------------
class DirectionalLight : public Light
{
public:
    DirectionalLight(Vec3 color = Vec3(1, 1, 1), float intensity = 1000.f, const Vec3& direction = Vec3(0, -1, 0));
    Ray getShadowRay(const Vec3& collisionPoint, float offset) const override;
    Vec3 getColor(const Vec3& collisionPoint) const override;
    Vec3 vectorFrom(const Vec3& collisionPoint) const override;

private:
    Vec3 _direction;
};

// -----------------
// Spotlight
// -----------------
class SpotLight : public Light
{
public:
    SpotLight(Vec3 color = Vec3(0, 0, 0), Vec3 position = Vec3(1, 1, 1), float intensity = 1000.f, const Vec3& direction = Vec3(0, -1, 0), float cosThetaP = .5f, float cosThetaU = .7f, float exp = 1);
    Ray getShadowRay(const Vec3& collisionPoint, float offset) const override;
    Vec3 getColor(const Vec3& collisionPoint) const override;
    Vec3 vectorFrom(const Vec3& collisionPoint) const override;

private:
    Vec3 _position;
    Vec3 _direction;
    float _cosThetaP, _cosThetaU, _exp; // thetaP denotes start of penumbra, thetaU denotes end of penumbra (start of umbra)
};
