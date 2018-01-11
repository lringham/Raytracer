#include "Vec2.h"
#include <cmath>

Vec2::Vec2(float val) : x(val), y(val)
{
}

Vec2::Vec2(float x, float y) : x(x), y(y)
{
}

float Vec2::length() const
{
	return std::sqrt(x*x + y*y);
}

void Vec2::set(float x, float y)
{
	this->x = x;
	this->y = y;
}

void Vec2::set(Vec2 v)
{
	this->x = v.x;
	this->y = v.y;
}

float Vec2::dot(Vec2 v)
{
	return x*v.x + y*v.y;
}

void Vec2::zero()
{
	x = 0;
	y = 0;
}

void Vec2::normalize()
{
	float len = length();

	if (len == 0)
		return;

	x = x / len;
	y = y / len;
}

void Vec2::rotate(float angle)
{
	float newX = cos(angle)*x - sin(angle)*y;
	float newY = sin(angle)*x + cos(angle)*y;
	x = newX;
	y = newY;
}

std::ostream& operator<<(std::ostream& os, const Vec2& v)
{
	os << "(" << v.x << ", " << v.y << ")";
	return os;
}

Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
	return Vec2(v1.x + v2.x, v1.y + v2.y);
}

Vec2 operator+(const float& val, const Vec2& v)
{
	return Vec2(v.x + val, v.y + val);
}

Vec2 operator+(const Vec2& v, const float& val)
{
	return Vec2(v.x + val, v.y + val);
}

Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
	return Vec2(v1.x - v2.x, v1.y - v2.y);
}

Vec2 operator-(const float& val, const Vec2& v)
{
	return Vec2(v.x - val, v.y - val);
}

Vec2 operator-(const Vec2& v, const float& val)
{
	return Vec2(v.x - val, v.y - val);
}

Vec2 operator*(const Vec2& v1, const Vec2& v2)
{
	return Vec2(v1.x * v2.x, v1.y * v2.y);
}

Vec2 operator*(const float& val, const Vec2& v)
{
	return Vec2(v.x * val, v.y * val);
}

Vec2 operator*(const Vec2& v, const float& val)
{
	return Vec2(v.x * val, v.y * val);
}

//TODO: Add non member funcs
Vec2 add(const Vec2& v1, const Vec2& v2)
{
	return Vec2(
		v1.x + v2.x,
		v1.y + v2.y);
}

Vec2 sub(const Vec2& v1, const Vec2& v2)
{
	return Vec2(
		v1.x - v2.x,
		v1.y - v2.y);
}

Vec2 mul(const Vec2& v, float val)
{
	return Vec2(
		v.x * val,
		v.y * val);
}

Vec2 div(const Vec2& v, float val)
{
	return Vec2(
		v.x / val,
		v.y / val);
}

float length(const Vec2& v)
{
	return sqrt(v.x*v.x + v.y*v.y);
}

Vec2 normalize(const Vec2& v)
{
	float len = length(v);
	if (len > 0)
		return div(v, len);
	else
		return v;
}
