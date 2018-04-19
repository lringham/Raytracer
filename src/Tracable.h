#pragma once
#include "Ray.h"
#include "Material.h"

class Tracable
{
public:
    Tracable();
    virtual ~Tracable();
    virtual bool raycast(Ray& ray) const;

    int _materialID;
};
