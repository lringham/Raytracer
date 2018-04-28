#pragma once
#include "Vec3.h"
#include <random>

inline float clamp(float v, float v1, float v2)
{
    return v < v1 ? v1 : v > v2 ? v2 : v;
}

inline Vec3 pointInCircle()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.f, 1.f);
    Vec3 result(0, 0, 0);

    do
    {
        result.set(dis(gen), dis(gen), dis(gen)); // Find point in box around the sphere
    } while(dot(result, result) >= 1.f); // Reject point if outside the sphere

    return result;
}

inline Vec3 pointInCircle(std::mt19937& gen)
{
    std::uniform_real_distribution<> dis(-1.f, 1.f);
    Vec3 result(0, 0, 0);

    do
    {
        result.set(dis(gen), dis(gen), dis(gen)); // Find point in box around the sphere
    } while(dot(result, result) >= 1.f); // Reject point if outside the sphere

    return result;
}

inline bool startsWith(const std::string& string, const std::string& start)
{	
    size_t smallest = std::min(string.size(), start.size());
    for(size_t i = 0; i < smallest; ++i)
        if(start[i] != string[i])
            return false;
    return true;
}

inline std::string greenText(const std::string& text)
{
    #ifdef __linux__
    return "\033[1;32m" + text + "\033[0m";
    #else
    return text;
    #endif

}

template<typename T>
inline bool inbetweenInc(T val, T lower, T upper)
{
    return val >= lower && val <= upper;
}

template<typename T>
inline bool inbetweenExc(T val, T lower, T upper)
{
    return val > lower && val < upper;
}

const float PI_ = 3.14159265f;