#ifndef VEC2_HPP
#define VEC2_HPP

#include <cmath>

/**
 * @brief Simple 2D Vector structure with utility math functions.
 */
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
    Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }

    float length() const {
        return std::sqrt(x * x + y * y);
    }

    Vec2 normalized() const {
        float len = length();
        if (len == 0.0f) return {0.0f, 0.0f};
        return {x / len, y / len};
    }
};

inline float dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

#endif // VEC2_HPP