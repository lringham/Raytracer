#include "Tracable.h"
#include <iostream>

Tracable::Tracable()
{}

Tracable::~Tracable()
{}

bool Tracable::raycast(Ray&) const
{
    return true;
}
