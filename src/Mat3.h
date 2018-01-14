#pragma once
#include <iostream>
#include "Vec3.h"

class Mat3
{
public:
	Mat3(float mainDiag=0);
	Mat3(float elements[9]);
	~Mat3();

	void identity();
	void zero();
	void transpose();
	void set(Mat3 m);
	void set(float newElements[9]);
	void setRow(int index, float x, float y, float z);
	void setRow(int index, Vec3 row);
	void setCol(int index, float x, float y, float z);
	void setCol(int index, Vec3 col);

	//operators
	friend std::ostream& operator<<(std::ostream& os, const Mat3& m);
	friend Mat3 operator+(const Mat3& m1, const Mat3& m2);
	friend Mat3 operator-(const Mat3& m1, const Mat3& m2);
	friend Mat3 operator*(const Mat3& m1, const Mat3& m2);
	friend Vec3 operator*(const Mat3& m, const Vec3& v);

	float _elements[9];
};
