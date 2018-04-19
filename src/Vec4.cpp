#include "Vec4.h"
#include <cmath>

Vec4::Vec4(float val) :
    _x(val), _y(val), _z(val), _w(val)
{
}

Vec4::Vec4(float x, float y, float z, float w) :
    _x(x), _y(y), _z(z), _w(w)
{
}

Vec4::Vec4(Vec3 v, float w) :
    _x(v._x), _y(v._y), _z(v._z), _w(w)
{
}

void Vec4::set(float x, float y, float z, float w)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
    this->_w = w;
}

void Vec4::set(const Vec4& v)
{
    _x = v._x;
    _y = v._y;
    _z = v._z;
    _w = v._w;
}

void Vec4::zero()
{
    _x = 0;
    _y = 0;
    _z = 0;
    _w = 0;
}

Vec3 Vec4::xyz()
{
    return Vec3(_x, _y, _z);
}

float Vec4::dot(const Vec4&  v) const
{
    return _x*v._x + _y*v._y + _z*v._z + _w*v._w;
}

void Vec4::add(float x, float y, float z, float w)
{
    this->_x += x;
    this->_y += y;
    this->_z += z;
    this->_w += w;
}

void Vec4::add(const Vec4& translation)
{
    this->_x += translation._x;
    this->_y += translation._y;
    this->_z += translation._z;
    this->_w += translation._w;
}

void Vec4::sub(float x, float y, float z, float w)
{
    this->_x -= x;
    this->_y -= y;
    this->_z -= z;
    this->_w -= w;
}

void Vec4::sub(const Vec4& translation)
{
    this->_x -= translation._x;
    this->_y -= translation._y;
    this->_z -= translation._z;
    this->_w -= translation._w;
}

void Vec4::normalize()
{
    float len = length();

    if (len == 0)
        return;

    _x = _x / len;
    _y = _y / len;
    _z = _z / len;
    _w = _w / len;
}

float Vec4::length() const
{
    return std::sqrt(_x*_x + _y*_y + _z*_z + _w*_w);
}

std::ostream& operator<<(std::ostream& os, const Vec4& v)
{
    os << "(" << v._x << ", " << v._y << ", " << v._z << ", " << v._w << ")";
    return os;
}

Vec4 operator+(const Vec4& v1, const Vec4& v2)
{
    return Vec4(v1._x + v2._x, v1._y + v2._y,v1._z + v2._z, v1._w + v2._w);
}

Vec4 operator+(const float& val, const Vec4& v)
{
    return Vec4(v._x + val, v._y + val, v._z + val, v._z + val);
}

Vec4 operator+(const Vec4& v, const float& val)
{
    return Vec4(v._x + val, v._y + val, v._z + val, v._z + val);
}

Vec4 operator-(const Vec4& v1, const Vec4& v2)
{
    return Vec4(v1._x - v2._x, v1._y - v2._y, v1._z - v2._z, v1._w - v2._w);
}

Vec4 operator-(const float& val, const Vec4& v)
{
    return Vec4(v._x - val, v._y - val, v._z - val, v._w - val);
}

Vec4 operator-(const Vec4& v, const float& val)
{
    return Vec4(v._x - val, v._y - val, v._z - val, v._w - val);
}

Vec4 operator*(const Vec4& v1, const Vec4& v2)
{
    return Vec4(v1._x * v2._x, v1._y * v2._y, v1._z * v2._z, v1._w * v2._w);
}

Vec4 operator*(const float& val, const Vec4& v)
{
    return Vec4(v._x * val, v._y * val, v._z * val, v._w * val);
}

Vec4 operator*(const Vec4& v, const float& val)
{
    return Vec4(v._x * val, v._y * val, v._z * val, v._w * val);
}

//TODO: Add non member funcs
