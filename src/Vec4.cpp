#include "Vec4.h"
#include <cmath>

Vec4::Vec4(float val) :
    x_(val), y_(val), z_(val), w_(val)
{
}

Vec4::Vec4(float x, float y, float z, float w) :
    x_(x), y_(y), z_(z), w_(w)
{
}

Vec4::Vec4(Vec3 v, float w) :
    x_(v.x_), y_(v.y_), z_(v.z_), w_(w)
{
}

void Vec4::set(float x, float y, float z, float w)
{
    this->x_ = x;
    this->y_ = y;
    this->z_ = z;
    this->w_ = w;
}

void Vec4::set(const Vec4& v)
{
    x_ = v.x_;
    y_ = v.y_;
    z_ = v.z_;
    w_ = v.w_;
}

void Vec4::zero()
{
    x_ = 0;
    y_ = 0;
    z_ = 0;
    w_ = 0;
}

Vec3 Vec4::xyz()
{
    return Vec3(x_, y_, z_);
}

float Vec4::dot(const Vec4&  v) const
{
    return x_*v.x_ + y_*v.y_ + z_*v.z_ + w_*v.w_;
}

void Vec4::add(float x, float y, float z, float w)
{
    this->x_ += x;
    this->y_ += y;
    this->z_ += z;
    this->w_ += w;
}

void Vec4::add(const Vec4& translation)
{
    this->x_ += translation.x_;
    this->y_ += translation.y_;
    this->z_ += translation.z_;
    this->w_ += translation.w_;
}

void Vec4::sub(float x, float y, float z, float w)
{
    this->x_ -= x;
    this->y_ -= y;
    this->z_ -= z;
    this->w_ -= w;
}

void Vec4::sub(const Vec4& translation)
{
    this->x_ -= translation.x_;
    this->y_ -= translation.y_;
    this->z_ -= translation.z_;
    this->w_ -= translation.w_;
}

void Vec4::normalize()
{
    float len = length();

    if (len == 0)
        return;

    x_ = x_ / len;
    y_ = y_ / len;
    z_ = z_ / len;
    w_ = w_ / len;
}

float Vec4::length() const
{
    return std::sqrt(x_*x_ + y_*y_ + z_*z_ + w_*w_);
}

std::ostream& operator<<(std::ostream& os, const Vec4& v)
{
    os << "(" << v.x_ << ", " << v.y_ << ", " << v.z_ << ", " << v.w_ << ")";
    return os;
}

Vec4 operator+(const Vec4& v1, const Vec4& v2)
{
    return Vec4(v1.x_ + v2.x_, v1.y_ + v2.y_,v1.z_ + v2.z_, v1.w_ + v2.w_);
}

Vec4 operator+(const float& val, const Vec4& v)
{
    return Vec4(v.x_ + val, v.y_ + val, v.z_ + val, v.z_ + val);
}

Vec4 operator+(const Vec4& v, const float& val)
{
    return Vec4(v.x_ + val, v.y_ + val, v.z_ + val, v.z_ + val);
}

Vec4 operator-(const Vec4& v1, const Vec4& v2)
{
    return Vec4(v1.x_ - v2.x_, v1.y_ - v2.y_, v1.z_ - v2.z_, v1.w_ - v2.w_);
}

Vec4 operator-(const float& val, const Vec4& v)
{
    return Vec4(v.x_ - val, v.y_ - val, v.z_ - val, v.w_ - val);
}

Vec4 operator-(const Vec4& v, const float& val)
{
    return Vec4(v.x_ - val, v.y_ - val, v.z_ - val, v.w_ - val);
}

Vec4 operator*(const Vec4& v1, const Vec4& v2)
{
    return Vec4(v1.x_ * v2.x_, v1.y_ * v2.y_, v1.z_ * v2.z_, v1.w_ * v2.w_);
}

Vec4 operator*(const float& val, const Vec4& v)
{
    return Vec4(v.x_ * val, v.y_ * val, v.z_ * val, v.w_ * val);
}

Vec4 operator*(const Vec4& v, const float& val)
{
    return Vec4(v.x_ * val, v.y_ * val, v.z_ * val, v.w_ * val);
}

//TODO: Add non member funcs
