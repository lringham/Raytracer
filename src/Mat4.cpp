#include "Mat4.h"

Mat4::Mat4(float mainDiag) : elements_{ mainDiag,0,0,0,
                                        0,mainDiag,0,0,
                                        0,0,mainDiag,0,
                                        0,0,0,mainDiag }
{
}

Mat4::Mat4(float elements_[16])
{
    for (int i = 0; i < 16; i++)
        this->elements_[i] = elements_[i];
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
    elements_[0] = 1.f;
    elements_[1] = 0.f;
    elements_[2] = 0.f;
    elements_[3] = 0.f;
    
    elements_[4] = 0.f;
    elements_[5] = 1.f;
    elements_[6] = 0.f;
    elements_[7] = 0.f;
    
    elements_[8] = 0.f;
    elements_[9] = 0.f;
    elements_[10] = 1.f;
    elements_[11] = 0.f;
    
    elements_[12] = 0.f;
    elements_[13] = 0.f;
    elements_[14] = 0.f;
    elements_[15] = 1.f;
    
}

void Mat4::zero()
{
    for (int i = 0; i < 16; i++)
        elements_[i] = 0;
}

void Mat4::set(float newElements[16])
{
    for (int i = 0; i < 16; ++i)
        elements_[i] = newElements[i];
}

void Mat4::set(Mat4 m)
{
    set(m.elements_);
}

void Mat4::setRow(int index, float x, float y, float z, float w)
{
    int off = 4 * index;
    
    elements_[off]     = x;
    elements_[off + 1] = y;
    elements_[off + 2] = z;
    elements_[off + 3] = w;
}

void Mat4::setRow(int index, Vec3 row, float w)
{
    int off = 4 * index;
    
    elements_[off]	  	= row.x_;
    elements_[off + 1] = row.y_;
    elements_[off + 2] = row.z_;
    elements_[off + 3] = w;
}

void Mat4::setCol(int index, float x, float y, float z, float w)
{
    elements_[0 + index] = x;
    elements_[4 + index] = y;
    elements_[8 + index] = z;
    elements_[12+ index] = w;
}

void Mat4::setCol(int index, Vec3 col, float w)
{
    elements_[0 + index] = col.x_;
    elements_[4 + index] = col.y_;
    elements_[8 + index] = col.z_;
    elements_[12 + index] = w;
}


void Mat4::transpose()
{
    //transpose
    float transposed[16] = {
        elements_[0], elements_[4], elements_[8], elements_[12],
        elements_[1], elements_[5], elements_[9], elements_[13],
        elements_[2], elements_[6], elements_[10], elements_[14],
        elements_[3], elements_[7], elements_[11], elements_[15]
    };
    
    //copy elements_
    for (int i = 0; i < 16; i++)
        elements_[i] = transposed[i];
}

// Opertors
std::ostream& operator<<(std::ostream& os, const Mat4& m)
{
    for (int i = 0; i < 4; i++)
    {
        os << "[";
        for (int j = 0; j < 4; j++)
        {
            os << m.elements_[i*4+j];
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
        m.elements_[i] = m1.elements_[i] + m2.elements_[i];
    return m;
}

Mat4 operator-(const Mat4& m1, const Mat4& m2)
{
    Mat4 m;
    for (int i = 0; i < 16; i++)
        m.elements_[i] = m1.elements_[i] - m2.elements_[i];
    return m;
}

Vec4 operator*(const Mat4& m, const Vec4& v)
{
    return Vec4(m.elements_[0]  * v.x_ + m.elements_[1]  * v.y_ + m.elements_[2]  * v.z_ + m.elements_[3]  * v.w_,
            m.elements_[4]  * v.x_ + m.elements_[5]  * v.y_ + m.elements_[6]  * v.z_ + m.elements_[7]  * v.w_,
            m.elements_[8]  * v.x_ + m.elements_[9]  * v.y_ + m.elements_[10] * v.z_ + m.elements_[11] * v.w_,
            m.elements_[12] * v.x_ + m.elements_[13] * v.y_ + m.elements_[14] * v.z_ + m.elements_[15] * v.w_);
}

Mat4 operator*(const Mat4& m1, const Mat4& m2)
{
    Mat4 m;
    m.elements_[0] = m1.elements_[0] * m2.elements_[0] + m1.elements_[1] * m2.elements_[4] + m1.elements_[2] * m2.elements_[8] + m1.elements_[3] * m2.elements_[12];
    m.elements_[1] = m1.elements_[0] * m2.elements_[1] + m1.elements_[1] * m2.elements_[5] + m1.elements_[2] * m2.elements_[9] + m1.elements_[3] * m2.elements_[13];
    m.elements_[2] = m1.elements_[0] * m2.elements_[2] + m1.elements_[1] * m2.elements_[6] + m1.elements_[2] * m2.elements_[10] + m1.elements_[3] * m2.elements_[14];
    m.elements_[3] = m1.elements_[0] * m2.elements_[3] + m1.elements_[1] * m2.elements_[7] + m1.elements_[2] * m2.elements_[11] + m1.elements_[3] * m2.elements_[15];
    
    m.elements_[4] = m1.elements_[4] * m2.elements_[0] + m1.elements_[5] * m2.elements_[4] + m1.elements_[6] * m2.elements_[8]  + m1.elements_[7] * m2.elements_[12];
    m.elements_[5] = m1.elements_[4] * m2.elements_[1] + m1.elements_[5] * m2.elements_[5] + m1.elements_[6] * m2.elements_[9]  + m1.elements_[7] * m2.elements_[13];
    m.elements_[6] = m1.elements_[4] * m2.elements_[2] + m1.elements_[5] * m2.elements_[6] + m1.elements_[6] * m2.elements_[10] + m1.elements_[7] * m2.elements_[14];
    m.elements_[7] = m1.elements_[4] * m2.elements_[3] + m1.elements_[5] * m2.elements_[7] + m1.elements_[6] * m2.elements_[11] + m1.elements_[7] * m2.elements_[15];
    
    m.elements_[8]  = m1.elements_[8] * m2.elements_[0] + m1.elements_[9] * m2.elements_[4] + m1.elements_[10] * m2.elements_[8]  + m1.elements_[11] * m2.elements_[12];
    m.elements_[9]  = m1.elements_[8] * m2.elements_[1] + m1.elements_[9] * m2.elements_[5] + m1.elements_[10] * m2.elements_[9]  + m1.elements_[11] * m2.elements_[13];
    m.elements_[10] = m1.elements_[8] * m2.elements_[2] + m1.elements_[9] * m2.elements_[6] + m1.elements_[10] * m2.elements_[10] + m1.elements_[11] * m2.elements_[14];
    m.elements_[11] = m1.elements_[8] * m2.elements_[3] + m1.elements_[9] * m2.elements_[7] + m1.elements_[10] * m2.elements_[11] + m1.elements_[11] * m2.elements_[15];
    
    m.elements_[12] = m1.elements_[12] * m2.elements_[0] + m1.elements_[13] * m2.elements_[4] + m1.elements_[14] * m2.elements_[8]  + m1.elements_[15] * m2.elements_[12];
    m.elements_[13] = m1.elements_[12] * m2.elements_[1] + m1.elements_[13] * m2.elements_[5] + m1.elements_[14] * m2.elements_[9]  + m1.elements_[15] * m2.elements_[13];
    m.elements_[14] = m1.elements_[12] * m2.elements_[2] + m1.elements_[13] * m2.elements_[6] + m1.elements_[14] * m2.elements_[10] + m1.elements_[15] * m2.elements_[14];
    m.elements_[15] = m1.elements_[12] * m2.elements_[3] + m1.elements_[13] * m2.elements_[7] + m1.elements_[14] * m2.elements_[11] + m1.elements_[15] * m2.elements_[15];
    return m;
}


// Non-member functions
Mat4 transpose(const Mat4& m)
{
    Mat4 mT;
    
    mT.elements_[0] = m.elements_[0];
    mT.elements_[1] = m.elements_[4];
    mT.elements_[2] = m.elements_[8];
    mT.elements_[3] = m.elements_[12];
    
    mT.elements_[4] = m.elements_[1];
    mT.elements_[5] = m.elements_[5];
    mT.elements_[6] = m.elements_[9];
    mT.elements_[7] = m.elements_[13];
    
    mT.elements_[8] = m.elements_[2];
    mT.elements_[9] = m.elements_[6];
    mT.elements_[10] = m.elements_[10];
    mT.elements_[11] = m.elements_[14];
    
    mT.elements_[12] = m.elements_[3];
    mT.elements_[13] = m.elements_[7];
    mT.elements_[14] = m.elements_[11];
    mT.elements_[15] = m.elements_[15];
    
    return mT;
}

//TODO: Add non member funcs
