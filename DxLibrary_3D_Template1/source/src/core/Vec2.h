#pragma once
#include <cmath>

struct Vec2 {
    float x = 0, y = 0;
    constexpr Vec2() = default;
    constexpr Vec2(float x_, float y_) : x(x_), y(y_) {}
    Vec2 operator+(Vec2 o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(Vec2 o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    Vec2 operator/(float s) const { return {x / s, y / s}; }
    Vec2 operator-() const { return {-x, -y}; }
    Vec2& operator+=(Vec2 o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(Vec2 o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }
    float len() const { return std::sqrt(x * x + y * y); }
    float len2() const { return x * x + y * y; }
    Vec2 norm() const { float l = len(); return l > 1e-6f ? Vec2{x / l, y / l} : Vec2{0, 0}; }
    float dot(Vec2 o) const { return x * o.x + y * o.y; }
    float angle() const { return std::atan2(y, x); }
    static Vec2 fromAngle(float a) { return {std::cos(a), std::sin(a)}; }
    Vec2 rotated(float a) const { float c = std::cos(a), s = std::sin(a); return {x * c - y * s, x * s + y * c}; }
};

inline float dist(Vec2 a, Vec2 b) { return (a - b).len(); }
inline float dist2(Vec2 a, Vec2 b) { return (a - b).len2(); }

constexpr float PI = 3.14159265358979f;
constexpr float DT = 1.0f / 60.0f;

template <class T> inline T clampv(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline float approach(float v, float target, float step) {
    if (v < target) return (v + step > target) ? target : v + step;
    return (v - step < target) ? target : v - step;
}
