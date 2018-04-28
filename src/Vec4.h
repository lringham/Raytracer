#pragma once
#include <iostream>
#include "Vec3.h"

class Vec4
{
public:
    Vec4(float val = 0);
    Vec4(float x, float y, float z, float w = 0.f);
    Vec4(Vec3 v, float w = 0.f);
    void zero();
    void set(float x, float y, float z, float w = 0.f);
    void set(const Vec4& v);

    float dot(const Vec4& v) const;
    void normalize();
    float length() const;
    Vec3 xyz();

    void sub(float x, float y, float z, float w);
    void sub(const Vec4& translation);
    void add(float x, float y, float z, float w);
    void add(const Vec4& translation);

    //FIXME: need to be friends?
    friend std::ostream& operator<<(std::ostream& os, const Vec4& v);
    friend Vec4 operator+(const Vec4& v1, const Vec4& v2);
    friend Vec4 operator+(const float& val, const Vec4& v);
    friend Vec4 operator+(const Vec4& v, const float& val);

    friend Vec4 operator-(const Vec4& v1, const Vec4& v2);
    friend Vec4 operator-(const float& val, const Vec4& v);
    friend Vec4 operator-(const Vec4& v, const float& val);

    friend Vec4 operator*(const Vec4& v1, const Vec4& v2);
    friend Vec4 operator*(const float& val, const Vec4& v);
    friend Vec4 operator*(const Vec4& v, const float& val);

    union
    {
        float x_, r_;
    };

    union
    {
        float y_, g_;
    };

    union
    {
        float z_, b_;
    };

    union
    {
        float w_, a_;
    };
};
