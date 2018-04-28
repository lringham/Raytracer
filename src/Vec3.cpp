#include "Vec3.h"
#include "Mat3.h"
#include <cmath>

Vec3::Vec3(float val) :
    x_(val), y_(val), z_(val)
{
}

Vec3::Vec3(float x, float y, float z) :
    x_(x), y_(y), z_(z)
{
}

void Vec3::set(float x, float y, float z)
{
    this->x_ =x;
    this->y_ =y;
    this->z_ =z;
}

void Vec3::set(const Vec3& v)
{
    x_ = v.x_;
    y_ = v.y_;
    z_ = v.z_;
}

void Vec3::zero()
{
    x_ = 0;
    y_ = 0;
    z_ = 0;
}

Vec3 Vec3::cross(const Vec3&  v) const
{
    return Vec3(
                y_*v.z_ - z_*v.y_,
                z_*v.x_ - x_*v.z_,
                x_*v.y_ - y_*v.x_);
}

float Vec3::dot(const Vec3&  v) const
{
    return x_*v.x_ + y_*v.y_ + z_*v.z_;
}

Vec3 Vec3::projOnto(const Vec3& v)
{
    Vec3 unitV(::normalize(v));
    return unitV * dot(unitV);
}

void Vec3::rotate(float angle, Vec3 axis)
{
    Vec3 v = *this;
    *this = v*cos(angle) + ::cross(axis, v)*sin(angle) + axis*(::dot(axis, v)*(1.f-cos(angle)));
}

float Vec3::areaBetween(const Vec3& v)
{
    return cross(v).length() / 2.f;
}

float Vec3::acuteAngleBetween(const Vec3& v)
{
    return acos(::dot(::normalize((*this)), ::normalize(v)));
}

void Vec3::add(float x, float y, float z)
{
    this->x_ += x;
    this->y_ += y;
    this->z_ += z;
}

void Vec3::add(const Vec3& translation)
{
    this->x_ += translation.x_;
    this->y_ += translation.y_;
    this->z_ += translation.z_;
}

void Vec3::sub(float x, float y, float z)
{
    this->x_ -= x;
    this->y_ -= y;
    this->z_ -= z;
}

void Vec3::sub(const Vec3& translation)
{
    this->x_ -= translation.x_;
    this->y_ -= translation.y_;
    this->z_ -= translation.z_;
}

void Vec3::normalize()
{
    float len = length();

    if (len == 0)
        return;

    x_ = x_ / len;
    y_ = y_ / len;
    z_ = z_ / len;
}

float Vec3::length() const
{
    return sqrt(x_*x_ + y_*y_ + z_*z_);
}

std::ostream& operator<<(std::ostream& os, const Vec3& v)
{
    os << "(" << v.x_ << ", " << v.y_ << ", " << v.z_ << ")";
    return os;
}

Vec3 operator+(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.x_ + v2.x_, v1.y_ + v2.y_, v1.z_ + v2.z_);
}

Vec3 operator+(const float& val, const Vec3& v)
{
    return Vec3(v.x_ + val, v.y_ + val, v.z_ + val);
}

Vec3 operator+(const Vec3& v, const float& val)
{
    return Vec3(v.x_ + val, v.y_ + val, v.z_ + val);
}

Vec3 operator-(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.x_ - v2.x_, v1.y_ - v2.y_, v1.z_ - v2.z_);
}

Vec3 operator-(const float& val, const Vec3& v)
{
    return Vec3(v.x_ - val, v.y_ - val, v.z_ - val);
}

Vec3 operator-(const Vec3& v, const float& val)
{
    return Vec3(v.x_ - val, v.y_ - val, v.z_ - val);
}

Vec3 operator*(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.x_ * v2.x_, v1.y_ * v2.y_, v1.z_ * v2.z_);
}

Vec3 operator*(const float& val, const Vec3& v)
{
    return Vec3(v.x_ * val, v.y_ * val, v.z_ * val);
}

Vec3 operator*(const Vec3& v, const float& val)
{
    return Vec3(v.x_ * val, v.y_ * val, v.z_ * val);
}

Vec3 operator/(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.x_ / v2.x_, v1.y_ / v2.y_, v1.z_ / v2.z_);
}

Vec3 operator/(const float& val, const Vec3& v)
{
    return Vec3(v.x_ / val, v.y_ / val, v.z_ / val);
}

Vec3 operator/(const Vec3& v, const float& val)
{
    return Vec3(v.x_ / val, v.y_ / val, v.z_ / val);
}

// Non member functions
Vec3 add(const Vec3& v1, const Vec3& v2)
{
    return Vec3(
                v1.x_ + v2.x_,
                v1.y_ + v2.y_,
                v1.z_ + v2.z_);
}

Vec3 sub(const Vec3& v1, const Vec3& v2)
{
    return Vec3(
                v1.x_ - v2.x_,
                v1.y_ - v2.y_,
                v1.z_ - v2.z_);
}

Vec3 mul(const Vec3& v, float val)
{
    return Vec3(
                v.x_ * val,
                v.y_ * val,
                v.z_ * val);
}

Vec3 div(const Vec3& v, float val)
{
    return Vec3(
                v.x_ / val,
                v.y_ / val,
                v.z_ / val);
}

float length(const Vec3& v)
{
    return sqrt(v.x_*v.x_ + v.y_*v.y_ + v.z_*v.z_);
}

float dot(const Vec3& v1, const Vec3& v2)
{
    return v1.x_ * v2.x_ +
            v1.y_ * v2.y_ +
            v1.z_ * v2.z_;
}

Vec3 cross(const Vec3& v1, const Vec3& v2)
{
    return Vec3(
                v1.y_*v2.z_ - v1.z_*v2.y_,
                v1.z_*v2.x_ - v1.x_*v2.z_,
                v1.x_*v2.y_ - v1.y_*v2.x_);
}

Vec3 normalize(const Vec3& v)
{
    float len = length(v);
    if (len > 0)
        return div(v, len);
    else
        return v;
}

Vec3 projOnto(const Vec3& v1, const Vec3& v2)
{
    Vec3 unitV2 = normalize(v2);
    return unitV2.dot(v1) * unitV2;
}

float areaBetween(const Vec3& v1, const Vec3& v2)
{
    return length(cross(v1, v2)) / 2.f;
}

float acuteAngleBetween(const Vec3& v1, const Vec3& v2)
{
    return acos(dot(normalize(v1), normalize(v2)));
}

Vec3 reflect(const Vec3& I, const Vec3& N)
{
    return I - 2.f*N.dot(I)*N;
}

bool refract(const Vec3& I, const Vec3& N, float eta1, float eta2, Vec3& result)
{
    float eta = eta1/eta2;
    float cosI = dot(N, I);
    float sinT2 = eta*eta*(1.f-cosI*cosI);
    if(sinT2 > 1.f || cosI > 0.f)
        return false;
    result = eta*(I-N*cosI) - std::sqrt(1.f-sinT2)*N;
    return true;
}

Vec3 rotate(const Vec3& v, float angle, const Vec3& axis)
{
    return v*cos(angle) + ::cross(axis, v)*sin(angle) + axis*(::dot(axis, v)*(1.f - cos(angle)));
}

Vec3 lerp(float s, const Vec3& v1, const Vec3& v2)
{
    return (1.f-s)*v1 + s*(v2);
}

Vec3 nlerp(float s, const Vec3& v1, const Vec3& v2)
{
    return normalize(s*(v2)+(1.f - s)*v1);
}

Vec3 slerp(float s, const Vec3& v1, const Vec3& v2)
{
    float theta = acuteAngleBetween(v1, v2);
    float sinTheta = sin(theta);
    return sin((1.f - s)*theta) / sinTheta * v1 + sin(s*theta) / sinTheta * v2;
}
