#include "Mat3.h"

Mat3::Mat3(float mainDiag) : elements{ mainDiag, 0, 0,
									   0, mainDiag, 0,
									   0, 0, mainDiag }
{
}

Mat3::Mat3(float elements[9])
{
	this->elements[0] = elements[0];
	this->elements[1] = elements[1];
	this->elements[2] = elements[2];

	this->elements[3] = elements[3];
	this->elements[4] = elements[4];
	this->elements[5] = elements[5];

	this->elements[6] = elements[6];
	this->elements[7] = elements[7];
	this->elements[8] = elements[8];
}

Mat3::~Mat3()
{
}

void Mat3::identity()
{
	elements[0] = 1.f;
	elements[1] = 0.f;
	elements[2] = 0.f;

	elements[3] = 0.f;
	elements[4] = 1.f;
	elements[5] = 0.f;

	elements[6] = 0.f;
	elements[7] = 0.f;
	elements[8] = 1.f;
}

void Mat3::zero()
{
	elements[0] = 0.f;
	elements[1] = 0.f;
	elements[2] = 0.f;

	elements[3] = 0.f;
	elements[4] = 0.f;
	elements[5] = 0.f;

	elements[6] = 0.f;
	elements[7] = 0.f;
	elements[8] = 0.f;
}

void Mat3::set(float newElements[9])
{
	for (int i = 0; i < 9; ++i)
		elements[i] = newElements[i];
}

void Mat3::set(Mat3 m)
{
	set(m.elements);
}

void Mat3::setRow(int index, float x, float y, float z)
{
	int off = 3 * index;
	elements[off] = x;
	elements[off + 1] = y;
	elements[off + 2] = z;
}

void Mat3::setRow(int index, Vec3 row)
{
	int off = 3 * index;
	elements[off] = row.x;
	elements[off + 1] = row.y;
	elements[off + 2] = row.z;
}

void Mat3::setCol(int index, float x, float y, float z)
{
	elements[0 + index] = x;
	elements[3 + index] = y;
	elements[6 + index] = z;
}

void Mat3::setCol(int index, Vec3 col)
{
	elements[0 + index] = col.x;
	elements[3 + index] = col.y;
	elements[6 + index] = col.z;
}


void Mat3::transpose()
{
	//transpose
	float transposed[9] = {
		elements[0], elements[3], elements[6],
		elements[1], elements[4], elements[7],
		elements[2], elements[5], elements[8]
	};

	//copy elements
	for (int i = 0; i < 9; i++)
		elements[i] = transposed[i];
}

// Opertors
std::ostream& operator<<(std::ostream& os, const Mat3& m)
{
	for (int i = 0; i < 3; i++)
	{
		os << "[";
		for (int j = 0; j < 3; j++)
		{
			os << m.elements[i * 3 + j];
			if (j != 2) os << " ";
		}
		os << "]" << std::endl;
	}
	return os;
}

Mat3 operator+(const Mat3& m1, const Mat3& m2)
{
	Mat3 m;
	for (int i = 0; i < 9; i++)
		m.elements[i] = m1.elements[i] + m2.elements[i];
	return m;
}

Mat3 operator-(const Mat3& m1, const Mat3& m2)
{
	Mat3 m;
	for (int i = 0; i < 9; i++)
		m.elements[i] = m1.elements[i] - m2.elements[i];
	return m;
}

Vec3 operator*(const Mat3& m, const Vec3& v)
{
	Vec3 ans;

	ans.x = m.elements[0] * v.x + m.elements[1] * v.y + m.elements[2] * v.z;
	ans.y = m.elements[3] * v.x + m.elements[4] * v.y + m.elements[5] * v.z;
	ans.z = m.elements[6] * v.x + m.elements[7] * v.y + m.elements[8] * v.z;

	return ans;
}

Mat3 operator*(const Mat3& m1, const Mat3& m2)
{
	Mat3 m;

	m.elements[0] = m1.elements[0] * m2.elements[0] + m1.elements[1] * m2.elements[3] + m1.elements[2] * m2.elements[6];
	m.elements[1] = m1.elements[0] * m2.elements[1] + m1.elements[1] * m2.elements[4] + m1.elements[2] * m2.elements[7];
	m.elements[2] = m1.elements[0] * m2.elements[2] + m1.elements[1] * m2.elements[5] + m1.elements[2] * m2.elements[8];

	m.elements[3] = m1.elements[3] * m2.elements[0] + m1.elements[4] * m2.elements[3] + m1.elements[5] * m2.elements[6];
	m.elements[4] = m1.elements[3] * m2.elements[1] + m1.elements[4] * m2.elements[4] + m1.elements[5] * m2.elements[7];
	m.elements[5] = m1.elements[3] * m2.elements[2] + m1.elements[4] * m2.elements[5] + m1.elements[5] * m2.elements[8];

	m.elements[6] = m1.elements[6] * m2.elements[0] + m1.elements[7] * m2.elements[3] + m1.elements[8] * m2.elements[6];
	m.elements[7] = m1.elements[6] * m2.elements[1] + m1.elements[7] * m2.elements[4] + m1.elements[8] * m2.elements[7];
	m.elements[8] = m1.elements[6] * m2.elements[2] + m1.elements[7] * m2.elements[5] + m1.elements[8] * m2.elements[8];

	return m;
}
