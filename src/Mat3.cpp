#include "Mat3.h"

Mat3::Mat3(float mainDiag) : elements_{ mainDiag, 0, 0,
                                        0, mainDiag, 0,
                                        0, 0, mainDiag }
{
}

Mat3::Mat3(float elements_[9])
{
    this->elements_[0] = elements_[0];
    this->elements_[1] = elements_[1];
    this->elements_[2] = elements_[2];

    this->elements_[3] = elements_[3];
    this->elements_[4] = elements_[4];
    this->elements_[5] = elements_[5];

    this->elements_[6] = elements_[6];
    this->elements_[7] = elements_[7];
    this->elements_[8] = elements_[8];
}

Mat3::~Mat3()
{
}

void Mat3::identity()
{
    elements_[0] = 1.f;
    elements_[1] = 0.f;
    elements_[2] = 0.f;

    elements_[3] = 0.f;
    elements_[4] = 1.f;
    elements_[5] = 0.f;

    elements_[6] = 0.f;
    elements_[7] = 0.f;
    elements_[8] = 1.f;
}

void Mat3::zero()
{
    elements_[0] = 0.f;
    elements_[1] = 0.f;
    elements_[2] = 0.f;

    elements_[3] = 0.f;
    elements_[4] = 0.f;
    elements_[5] = 0.f;

    elements_[6] = 0.f;
    elements_[7] = 0.f;
    elements_[8] = 0.f;
}

void Mat3::set(float newElements[9])
{
    for (int i = 0; i < 9; ++i)
        elements_[i] = newElements[i];
}

void Mat3::set(Mat3 m)
{
    set(m.elements_);
}

void Mat3::setRow(int index, float x, float y, float z)
{
    int off = 3 * index;
    elements_[off] = x;
    elements_[off + 1] = y;
    elements_[off + 2] = z;
}

void Mat3::setRow(int index, Vec3 row)
{
    int off = 3 * index;
    elements_[off] 		= row.x_;
    elements_[off + 1] = row.y_;
    elements_[off + 2] = row.z_;
}

void Mat3::setCol(int index, float x, float y, float z)
{
    elements_[0 + index] = x;
    elements_[3 + index] = y;
    elements_[6 + index] = z;
}

void Mat3::setCol(int index, Vec3 col)
{
    elements_[0 + index] = col.x_;
    elements_[3 + index] = col.y_;
    elements_[6 + index] = col.z_;
}


void Mat3::transpose()
{
    //transpose
    float transposed[9] = {
        elements_[0], elements_[3], elements_[6],
        elements_[1], elements_[4], elements_[7],
        elements_[2], elements_[5], elements_[8]
    };

    //copy elements_
    for (int i = 0; i < 9; i++)
        elements_[i] = transposed[i];
}

// Opertors
std::ostream& operator<<(std::ostream& os, const Mat3& m)
{
    for (int i = 0; i < 3; i++)
    {
        os << "[";
        for (int j = 0; j < 3; j++)
        {
            os << m.elements_[i * 3 + j];
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
        m.elements_[i] = m1.elements_[i] + m2.elements_[i];
    return m;
}

Mat3 operator-(const Mat3& m1, const Mat3& m2)
{
    Mat3 m;
    for (int i = 0; i < 9; i++)
        m.elements_[i] = m1.elements_[i] - m2.elements_[i];
    return m;
}

Vec3 operator*(const Mat3& m, const Vec3& v)
{
    Vec3 ans;

    ans.x_ = m.elements_[0] * v.x_ + m.elements_[1] * v.y_ + m.elements_[2] * v.z_;
    ans.y_ = m.elements_[3] * v.x_ + m.elements_[4] * v.y_ + m.elements_[5] * v.z_;
    ans.z_ = m.elements_[6] * v.x_ + m.elements_[7] * v.y_ + m.elements_[8] * v.z_;

    return ans;
}

Mat3 operator*(const Mat3& m1, const Mat3& m2)
{
    Mat3 m;

    m.elements_[0] = m1.elements_[0] * m2.elements_[0] + m1.elements_[1] * m2.elements_[3] + m1.elements_[2] * m2.elements_[6];
    m.elements_[1] = m1.elements_[0] * m2.elements_[1] + m1.elements_[1] * m2.elements_[4] + m1.elements_[2] * m2.elements_[7];
    m.elements_[2] = m1.elements_[0] * m2.elements_[2] + m1.elements_[1] * m2.elements_[5] + m1.elements_[2] * m2.elements_[8];

    m.elements_[3] = m1.elements_[3] * m2.elements_[0] + m1.elements_[4] * m2.elements_[3] + m1.elements_[5] * m2.elements_[6];
    m.elements_[4] = m1.elements_[3] * m2.elements_[1] + m1.elements_[4] * m2.elements_[4] + m1.elements_[5] * m2.elements_[7];
    m.elements_[5] = m1.elements_[3] * m2.elements_[2] + m1.elements_[4] * m2.elements_[5] + m1.elements_[5] * m2.elements_[8];

    m.elements_[6] = m1.elements_[6] * m2.elements_[0] + m1.elements_[7] * m2.elements_[3] + m1.elements_[8] * m2.elements_[6];
    m.elements_[7] = m1.elements_[6] * m2.elements_[1] + m1.elements_[7] * m2.elements_[4] + m1.elements_[8] * m2.elements_[7];
    m.elements_[8] = m1.elements_[6] * m2.elements_[2] + m1.elements_[7] * m2.elements_[5] + m1.elements_[8] * m2.elements_[8];

    return m;
}
