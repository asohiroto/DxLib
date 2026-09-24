#pragma once
#include <DxLib.h>
#include <cassert>
#include <cmath>

class Vec3
{
  public:
    float x;
    float y;
    float z;

  public:
    constexpr Vec3() : x(0.0f), y(0.0f), z(0.0f)
    {
    }

    constexpr Vec3(float fx, float fy, float fz) : x(fx), y(fy), z(fz)
    {
    }

    Vec3 operator+() const
    {
        return *this;
    }

    Vec3 operator-() const
    {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator+(const Vec3 &vec) const
    {
        return Vec3(x + vec.x, y + vec.y, z + vec.z);
    }

    Vec3 operator-(const Vec3 &vec) const
    {
        return Vec3(x - vec.x, y - vec.y, z - vec.z);
    }

    Vec3 operator*(float scale) const
    {
        return Vec3(x * scale, y * scale, z * scale);
    }

    Vec3 operator/(float scale) const
    {
        assert(scale != 0.0f);
        return Vec3(x / scale, y / scale, z / scale);
    }

    Vec3 operator+=(const Vec3 &vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;

        return *this;
    }

    Vec3 operator-=(const Vec3 &vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;

        return *this;
    }

    Vec3 operator*=(float scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;

        return *this;
    }

    Vec3 operator/=(float scale)
    {
        assert(scale != 0.0f);

        x /= scale;
        y /= scale;
        z /= scale;

        return *this;
    }

    bool operator==(const Vec3 &vec) const
    {
        if (x == vec.x && y == vec.y && z == vec.z)
            return true;
        else
            return false;
    }

    float SqLength() const
    {
        return x * x + y * y + z * z;
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
            z /= len;
        }
    }

    Vec3 GetNormalize() const
    {
        float len = Length();

        if (len > 0.0f)
        {
            return Vec3(x / len, y / len, z / len);
        }
        return Vec3();
    }

    static Vec3 Zero()
    {
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    static Vec3 Up()
    {
        return Vec3(0.0f, 1.0f, 0.0f);
    }

    static Vec3 Down()
    {
        return Vec3(0.0f, -1.0f, 0.0f);
    }

    static Vec3 Right()
    {
        return Vec3(1.0f, 0.0f, 0.0f);
    }

    static Vec3 Left()
    {
        return Vec3(-1.0f, 0.0f, 0.0f);
    }

    static Vec3 Forward()
    {
        return Vec3(0.0f, 0.0f, 1.0f);
    }

    static Vec3 Back()
    {
        return Vec3(0.0f, 0.0f, -1.0f);
    }

    float Dot(const Vec3 &vec) const
    {
        return x * vec.x + y * vec.y + z * vec.z;
    }

    Vec3 Cross(const Vec3 &vec) const
    {
        return Vec3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
    }

    static VECTOR ToVEC(const Vec3 &vec)
    {
        return VGet(vec.x, vec.y, vec.z);
    }

    static Vec3 ToVec(const VECTOR &vec)
    {
        return Vec3(vec.x, vec.y, vec.z);
    }
};
