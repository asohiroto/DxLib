#pragma once
#include <cassert>
#include <cmath>

class Vec2
{
  public:
    float x;
    float y;

  public:
    constexpr Vec2() : x(0.0f), y(0.0f)
    {
    }

    constexpr Vec2(float fx, float fy) : x(fx), y(fy)
    {
    }

    Vec2 operator+() const
    {
        return *this;
    }

    Vec2 operator-() const
    {
        return Vec2(-x, -y);
    }

    Vec2 operator+(const Vec2 &vec) const
    {
        return Vec2(x + vec.x, y + vec.y);
    }

    Vec2 operator-(const Vec2 &vec) const
    {
        return Vec2(x - vec.x, y - vec.y);
    }

    Vec2 operator*(float scale) const
    {
        return Vec2(x * scale, y * scale);
    }

    Vec2 operator/(float scale) const
    {
        assert(scale != 0.0f);
        return Vec2(x / scale, y / scale);
    }

    Vec2 &operator+=(const Vec2 &vec)
    {
        x += vec.x;
        y += vec.y;

        return *this;
    }

    Vec2 &operator-=(const Vec2 &vec)
    {
        x -= vec.x;
        y -= vec.y;

        return *this;
    }

    Vec2 &operator*=(float scale)
    {
        x *= scale;
        y *= scale;

        return *this;
    }

    Vec2 &operator/=(float scale)
    {
        assert(scale != 0.0f);

        x /= scale;
        y /= scale;

        return *this;
    }

    bool operator==(const Vec2 &vec) const
    {
        if (x == vec.x && y == vec.y)
            return true;
        else
            return false;
    }

    float SqLength() const
    {
        return x * x + y * y;
    }

    float Length() const
    {
        return sqrtf(SqLength());
    }

    void Normalize()
    {
        float len = Length();

        if (len > 0.0f)
        {
            x /= len;
            y /= len;
        }
    }

    Vec2 GetNormalize() const
    {
        float len = Length();

        if (len > 0.0f)
        {
            return Vec2(x / len, y / len);
        }
        return Vec2();
    }

    static Vec2 Zero()
    {
        return Vec2(0.0f, 0.0f);
    }

    // 画面座標系(y軸下向き)
    static Vec2 Up()
    {
        return Vec2(0.0f, -1.0f);
    }

    static Vec2 Down()
    {
        return Vec2(0.0f, 1.0f);
    }

    static Vec2 Right()
    {
        return Vec2(1.0f, 0.0f);
    }

    static Vec2 Left()
    {
        return Vec2(-1.0f, 0.0f);
    }

    float Dot(const Vec2 &vec) const
    {
        return x * vec.x + y * vec.y;
    }

    // 正ならvecが左側
    float Cross(const Vec2 &vec) const
    {
        return x * vec.y - y * vec.x;
    }

    static Vec2 Lerp(const Vec2 &start, const Vec2 &goal, float t)
    {
        if (t <= 0.0f)
            t = 0.0f;
        if (t >= 1.0f)
            t = 1.0f;

        return start + (goal - start) * t;
    }
};
