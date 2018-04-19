#include "Vec2.h"
#include <cmath>

Vec2::Vec2(float val)
    : _x(val), _y(val)
{
}

Vec2::Vec2(float x, float y)
    : _x(x), _y(y)
{
}

float Vec2::length() const
{
    return std::sqrt(_x*_x + _y*_y);
}

void Vec2::set(float x, float y)
{
    this->_x = x;
    this->_y = y;
}

void Vec2::set(Vec2 v)
{
    this->_x = v._x;
    this->_y = v._y;
}

float Vec2::dot(Vec2 v)
{
    return _x*v._x + _y*v._y;
}

void Vec2::zero()
{
    _x = 0;
    _y = 0;
}

void Vec2::normalize()
{
    float len = length();

    if (len == 0)
        return;

    _x = _x / len;
    _y = _y / len;
}

void Vec2::rotate(float angle)
{
    float newX = cos(angle)*_x - sin(angle)*_y;
    float newY = sin(angle)*_x + cos(angle)*_y;
    _x = newX;
    _y = newY;
}

std::ostream& operator<<(std::ostream& os, const Vec2& v)
{
    os << "(" << v._x << ", " << v._y << ")";
    return os;
}

Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1._x + v2._x, v1._y + v2._y);
}

Vec2 operator+(const float& val, const Vec2& v)
{
    return Vec2(v._x + val, v._y + val);
}

Vec2 operator+(const Vec2& v, const float& val)
{
    return Vec2(v._x + val, v._y + val);
}

Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1._x - v2._x, v1._y - v2._y);
}

Vec2 operator-(const float& val, const Vec2& v)
{
    return Vec2(v._x - val, v._y - val);
}

Vec2 operator-(const Vec2& v, const float& val)
{
    return Vec2(v._x - val, v._y - val);
}

Vec2 operator*(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1._x * v2._x, v1._y * v2._y);
}

Vec2 operator*(const float& val, const Vec2& v)
{
    return Vec2(v._x * val, v._y * val);
}

Vec2 operator*(const Vec2& v, const float& val)
{
    return Vec2(v._x * val, v._y * val);
}

//TODO: Add non member funcs
Vec2 add(const Vec2& v1, const Vec2& v2)
{
    return Vec2(
                v1._x + v2._x,
                v1._y + v2._y);
}

Vec2 sub(const Vec2& v1, const Vec2& v2)
{
    return Vec2(
                v1._x - v2._x,
                v1._y - v2._y);
}

Vec2 mul(const Vec2& v, float val)
{
    return Vec2(
                v._x * val,
                v._y * val);
}

Vec2 div(const Vec2& v, float val)
{
    return Vec2(
                v._x / val,
                v._y / val);
}

float length(const Vec2& v)
{
    return sqrt(v._x*v._x + v._y*v._y);
}

Vec2 normalize(const Vec2& v)
{
    float len = length(v);
    if (len > 0)
        return div(v, len);
    else
        return v;
}
