#pragma once
#include <iostream>

class Vec3
{
public:
	Vec3(float val = 0);
	Vec3(float x, float y, float z);

	void zero();
	void set(float x, float y, float z);
	void set(const Vec3& v);

	float dot(const Vec3& v) const;
	Vec3 cross(const Vec3& v) const;
	void normalize();
	float length() const;
	void rotate(float angle, Vec3 axis);

	void sub(float x, float y, float z);
	void sub(const Vec3& translation);
	void add(float x, float y, float z);
	void add(const Vec3& translation);
	Vec3 projOnto(const Vec3& v);
	float areaBetween(const Vec3& v);
	float acuteAngleBetween(const Vec3& v);
	Vec3 sampleBackground(const Vec3 dir) const;

	friend std::ostream& operator<<(std::ostream& os, const Vec3& v);
	friend Vec3 operator+(const Vec3& v1, const Vec3& v2);
	friend Vec3 operator+(const float& val, const Vec3& v);
	friend Vec3 operator+(const Vec3& v, const float& val);

	friend Vec3 operator-(const Vec3& v1, const Vec3& v2);
	friend Vec3 operator-(const float& val, const Vec3& v);
	friend Vec3 operator-(const Vec3& v, const float& val);

	friend Vec3 operator*(const Vec3& v1, const Vec3& v2);
	friend Vec3 operator*(const float& val, const Vec3& v);
	friend Vec3 operator*(const Vec3& v, const float& val);

	friend Vec3 operator/(const Vec3& v1, const Vec3& v2);
	friend Vec3 operator/(const float& val, const Vec3& v);
	friend Vec3 operator/(const Vec3& v, const float& val);

	union
	{
		float _x, _r, _u;
	};

	union
	{
		float _y, _g, _v;
	};

	union
	{
		float _z, _b, _w;
	};
};

Vec3 add(const Vec3& v1, const Vec3& v2);
Vec3 sub(const Vec3& v1, const Vec3& v2);
Vec3 mul(const Vec3& v, float val);
Vec3 div(const Vec3& v, float val);
float length(const Vec3& v);
float dot(const Vec3& v1, const Vec3& v2);
Vec3 cross(const Vec3& v1, const Vec3& v2);
Vec3 normalize(const Vec3& v);
Vec3 projOnto(const Vec3& v1, const Vec3& v2);
float areaBetween(const Vec3& v1, const Vec3& v2);
float acuteAngleBetween(const Vec3& v1, const Vec3& v2);
Vec3 reflect(const Vec3& I, const Vec3& N);
bool refract(const Vec3& I, const Vec3& N, float eta1, float eta2, Vec3& result);
Vec3 rotate(const Vec3& v, float angle, const Vec3& axis);
Vec3 lerp(float s, const Vec3& v1, const Vec3& v2);
Vec3 nlerp(float s, const Vec3& v1, const Vec3& v2);
Vec3 slerp(float s, const Vec3& v1, const Vec3& v2);
