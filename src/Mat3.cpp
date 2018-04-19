#include "Mat3.h"

Mat3::Mat3(float mainDiag) : _elements{ mainDiag, 0, 0,
                                        0, mainDiag, 0,
                                        0, 0, mainDiag }
{
}

Mat3::Mat3(float _elements[9])
{
    this->_elements[0] = _elements[0];
    this->_elements[1] = _elements[1];
    this->_elements[2] = _elements[2];

    this->_elements[3] = _elements[3];
    this->_elements[4] = _elements[4];
    this->_elements[5] = _elements[5];

    this->_elements[6] = _elements[6];
    this->_elements[7] = _elements[7];
    this->_elements[8] = _elements[8];
}

Mat3::~Mat3()
{
}

void Mat3::identity()
{
    _elements[0] = 1.f;
    _elements[1] = 0.f;
    _elements[2] = 0.f;

    _elements[3] = 0.f;
    _elements[4] = 1.f;
    _elements[5] = 0.f;

    _elements[6] = 0.f;
    _elements[7] = 0.f;
    _elements[8] = 1.f;
}

void Mat3::zero()
{
    _elements[0] = 0.f;
    _elements[1] = 0.f;
    _elements[2] = 0.f;

    _elements[3] = 0.f;
    _elements[4] = 0.f;
    _elements[5] = 0.f;

    _elements[6] = 0.f;
    _elements[7] = 0.f;
    _elements[8] = 0.f;
}

void Mat3::set(float newElements[9])
{
    for (int i = 0; i < 9; ++i)
        _elements[i] = newElements[i];
}

void Mat3::set(Mat3 m)
{
    set(m._elements);
}

void Mat3::setRow(int index, float x, float y, float z)
{
    int off = 3 * index;
    _elements[off] = x;
    _elements[off + 1] = y;
    _elements[off + 2] = z;
}

void Mat3::setRow(int index, Vec3 row)
{
    int off = 3 * index;
    _elements[off] 		= row._x;
    _elements[off + 1] = row._y;
    _elements[off + 2] = row._z;
}

void Mat3::setCol(int index, float x, float y, float z)
{
    _elements[0 + index] = x;
    _elements[3 + index] = y;
    _elements[6 + index] = z;
}

void Mat3::setCol(int index, Vec3 col)
{
    _elements[0 + index] = col._x;
    _elements[3 + index] = col._y;
    _elements[6 + index] = col._z;
}


void Mat3::transpose()
{
    //transpose
    float transposed[9] = {
        _elements[0], _elements[3], _elements[6],
        _elements[1], _elements[4], _elements[7],
        _elements[2], _elements[5], _elements[8]
    };

    //copy _elements
    for (int i = 0; i < 9; i++)
        _elements[i] = transposed[i];
}

// Opertors
std::ostream& operator<<(std::ostream& os, const Mat3& m)
{
    for (int i = 0; i < 3; i++)
    {
        os << "[";
        for (int j = 0; j < 3; j++)
        {
            os << m._elements[i * 3 + j];
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
        m._elements[i] = m1._elements[i] + m2._elements[i];
    return m;
}

Mat3 operator-(const Mat3& m1, const Mat3& m2)
{
    Mat3 m;
    for (int i = 0; i < 9; i++)
        m._elements[i] = m1._elements[i] - m2._elements[i];
    return m;
}

Vec3 operator*(const Mat3& m, const Vec3& v)
{
    Vec3 ans;

    ans._x = m._elements[0] * v._x + m._elements[1] * v._y + m._elements[2] * v._z;
    ans._y = m._elements[3] * v._x + m._elements[4] * v._y + m._elements[5] * v._z;
    ans._z = m._elements[6] * v._x + m._elements[7] * v._y + m._elements[8] * v._z;

    return ans;
}

Mat3 operator*(const Mat3& m1, const Mat3& m2)
{
    Mat3 m;

    m._elements[0] = m1._elements[0] * m2._elements[0] + m1._elements[1] * m2._elements[3] + m1._elements[2] * m2._elements[6];
    m._elements[1] = m1._elements[0] * m2._elements[1] + m1._elements[1] * m2._elements[4] + m1._elements[2] * m2._elements[7];
    m._elements[2] = m1._elements[0] * m2._elements[2] + m1._elements[1] * m2._elements[5] + m1._elements[2] * m2._elements[8];

    m._elements[3] = m1._elements[3] * m2._elements[0] + m1._elements[4] * m2._elements[3] + m1._elements[5] * m2._elements[6];
    m._elements[4] = m1._elements[3] * m2._elements[1] + m1._elements[4] * m2._elements[4] + m1._elements[5] * m2._elements[7];
    m._elements[5] = m1._elements[3] * m2._elements[2] + m1._elements[4] * m2._elements[5] + m1._elements[5] * m2._elements[8];

    m._elements[6] = m1._elements[6] * m2._elements[0] + m1._elements[7] * m2._elements[3] + m1._elements[8] * m2._elements[6];
    m._elements[7] = m1._elements[6] * m2._elements[1] + m1._elements[7] * m2._elements[4] + m1._elements[8] * m2._elements[7];
    m._elements[8] = m1._elements[6] * m2._elements[2] + m1._elements[7] * m2._elements[5] + m1._elements[8] * m2._elements[8];

    return m;
}
