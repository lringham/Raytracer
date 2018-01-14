#include "Mat4.h"

Mat4::Mat4(float mainDiag) : _elements{ mainDiag,0,0,0,
									   0,mainDiag,0,0,
									   0,0,mainDiag,0,
									   0,0,0,mainDiag }
{
}

Mat4::Mat4(float _elements[16])
{
	for (int i = 0; i < 16; i++)
		this->_elements[i] = _elements[i];
}

Mat4::Mat4(Vec3 row1, Vec3 row2, Vec3 row3, Vec3 row4)
{
	setRow(0, row1);
	setRow(1, row2);
	setRow(2, row3);
	setRow(3, row4, 1);
}

void Mat4::identity()
{
	_elements[0] = 1.f;
	_elements[1] = 0.f;
	_elements[2] = 0.f;
	_elements[3] = 0.f;

	_elements[4] = 0.f;
	_elements[5] = 1.f;
	_elements[6] = 0.f;
	_elements[7] = 0.f;

	_elements[8] = 0.f;
	_elements[9] = 0.f;
	_elements[10] = 1.f;
	_elements[11] = 0.f;

	_elements[12] = 0.f;
	_elements[13] = 0.f;
	_elements[14] = 0.f;
	_elements[15] = 1.f;

}

void Mat4::zero()
{
	for (int i = 0; i < 16; i++)
		_elements[i] = 0;
}

void Mat4::set(float newElements[16])
{
	for (int i = 0; i < 16; ++i)
		_elements[i] = newElements[i];
}

void Mat4::set(Mat4 m)
{
	set(m._elements);
}

void Mat4::setRow(int index, float x, float y, float z, float w)
{
	int off = 4 * index;

	_elements[off]     = x;
	_elements[off + 1] = y;
	_elements[off + 2] = z;
	_elements[off + 3] = w;
}

void Mat4::setRow(int index, Vec3 row, float w)
{
	int off = 4 * index;

	_elements[off]	  	= row._x;
	_elements[off + 1] = row._y;
	_elements[off + 2] = row._z;
	_elements[off + 3] = w;
}

void Mat4::setCol(int index, float x, float y, float z, float w)
{
	_elements[0 + index] = x;
	_elements[4 + index] = y;
	_elements[8 + index] = z;
	_elements[12+ index] = w;
}

void Mat4::setCol(int index, Vec3 col, float w)
{
	_elements[0 + index] = col._x;
	_elements[4 + index] = col._y;
	_elements[8 + index] = col._z;
	_elements[12 + index] = w;
}


void Mat4::transpose()
{
	//transpose
	float transposed[16] = {
		_elements[0], _elements[4], _elements[8], _elements[12],
		_elements[1], _elements[5], _elements[9], _elements[13],
		_elements[2], _elements[6], _elements[10], _elements[14],
		_elements[3], _elements[7], _elements[11], _elements[15]
	};

	//copy _elements
	for (int i = 0; i < 16; i++)
		_elements[i] = transposed[i];
}

// Opertors
std::ostream& operator<<(std::ostream& os, const Mat4& m)
{
	for (int i = 0; i < 4; i++)
	{
		os << "[";
		for (int j = 0; j < 4; j++)
		{
			os << m._elements[i*4+j];
			if (j != 3) os << " ";
		}
		os << "]" << std::endl;
	}
	return os;
}

Mat4 operator+(const Mat4& m1, const Mat4& m2)
{
	Mat4 m;
	for (int i = 0; i < 16; i++)
		m._elements[i] = m1._elements[i] + m2._elements[i];
	return m;
}

Mat4 operator-(const Mat4& m1, const Mat4& m2)
{
	Mat4 m;
	for (int i = 0; i < 16; i++)
		m._elements[i] = m1._elements[i] - m2._elements[i];
	return m;
}

Vec4 operator*(const Mat4& m, const Vec4& v)
{
	return Vec4(m._elements[0]  * v._x + m._elements[1]  * v._y + m._elements[2]  * v._z + m._elements[3]  * v._w,
				m._elements[4]  * v._x + m._elements[5]  * v._y + m._elements[6]  * v._z + m._elements[7]  * v._w,
				m._elements[8]  * v._x + m._elements[9]  * v._y + m._elements[10] * v._z + m._elements[11] * v._w,
				m._elements[12] * v._x + m._elements[13] * v._y + m._elements[14] * v._z + m._elements[15] * v._w);
}

Mat4 operator*(const Mat4& m1, const Mat4& m2)
{
	Mat4 m;
	m._elements[0] = m1._elements[0] * m2._elements[0] + m1._elements[1] * m2._elements[4] + m1._elements[2] * m2._elements[8] + m1._elements[3] * m2._elements[12];
	m._elements[1] = m1._elements[0] * m2._elements[1] + m1._elements[1] * m2._elements[5] + m1._elements[2] * m2._elements[9] + m1._elements[3] * m2._elements[13];
	m._elements[2] = m1._elements[0] * m2._elements[2] + m1._elements[1] * m2._elements[6] + m1._elements[2] * m2._elements[10] + m1._elements[3] * m2._elements[14];
	m._elements[3] = m1._elements[0] * m2._elements[3] + m1._elements[1] * m2._elements[7] + m1._elements[2] * m2._elements[11] + m1._elements[3] * m2._elements[15];

	m._elements[4] = m1._elements[4] * m2._elements[0] + m1._elements[5] * m2._elements[4] + m1._elements[6] * m2._elements[8]  + m1._elements[7] * m2._elements[12];
	m._elements[5] = m1._elements[4] * m2._elements[1] + m1._elements[5] * m2._elements[5] + m1._elements[6] * m2._elements[9]  + m1._elements[7] * m2._elements[13];
	m._elements[6] = m1._elements[4] * m2._elements[2] + m1._elements[5] * m2._elements[6] + m1._elements[6] * m2._elements[10] + m1._elements[7] * m2._elements[14];
	m._elements[7] = m1._elements[4] * m2._elements[3] + m1._elements[5] * m2._elements[7] + m1._elements[6] * m2._elements[11] + m1._elements[7] * m2._elements[15];

	m._elements[8]  = m1._elements[8] * m2._elements[0] + m1._elements[9] * m2._elements[4] + m1._elements[10] * m2._elements[8]  + m1._elements[11] * m2._elements[12];
	m._elements[9]  = m1._elements[8] * m2._elements[1] + m1._elements[9] * m2._elements[5] + m1._elements[10] * m2._elements[9]  + m1._elements[11] * m2._elements[13];
	m._elements[10] = m1._elements[8] * m2._elements[2] + m1._elements[9] * m2._elements[6] + m1._elements[10] * m2._elements[10] + m1._elements[11] * m2._elements[14];
	m._elements[11] = m1._elements[8] * m2._elements[3] + m1._elements[9] * m2._elements[7] + m1._elements[10] * m2._elements[11] + m1._elements[11] * m2._elements[15];

	m._elements[12] = m1._elements[12] * m2._elements[0] + m1._elements[13] * m2._elements[4] + m1._elements[14] * m2._elements[8]  + m1._elements[15] * m2._elements[12];
	m._elements[13] = m1._elements[12] * m2._elements[1] + m1._elements[13] * m2._elements[5] + m1._elements[14] * m2._elements[9]  + m1._elements[15] * m2._elements[13];
	m._elements[14] = m1._elements[12] * m2._elements[2] + m1._elements[13] * m2._elements[6] + m1._elements[14] * m2._elements[10] + m1._elements[15] * m2._elements[14];
	m._elements[15] = m1._elements[12] * m2._elements[3] + m1._elements[13] * m2._elements[7] + m1._elements[14] * m2._elements[11] + m1._elements[15] * m2._elements[15];
	return m;
}


// Non-member functions
Mat4 transpose(const Mat4& m)
{
	Mat4 mT;

	mT._elements[0] = m._elements[0];
	mT._elements[1] = m._elements[4];
	mT._elements[2] = m._elements[8];
	mT._elements[3] = m._elements[12];

	mT._elements[4] = m._elements[1];
	mT._elements[5] = m._elements[5];
	mT._elements[6] = m._elements[9];
	mT._elements[7] = m._elements[13];

	mT._elements[8] = m._elements[2];
	mT._elements[9] = m._elements[6];
	mT._elements[10] = m._elements[10];
	mT._elements[11] = m._elements[14];

	mT._elements[12] = m._elements[3];
	mT._elements[13] = m._elements[7];
	mT._elements[14] = m._elements[11];
	mT._elements[15] = m._elements[15];

	return mT;
}

//TODO: Add non member funcs
