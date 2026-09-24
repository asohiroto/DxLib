#pragma once
#include "Vec3.h"
#include <DxLib.h>
#include <cassert>
#include <cmath>

class Matrix4
{
  public:
    float m[4][4] = {};

  public:
    // 書き込み用
    float &operator()(int row, int col)
    {
        assert(0 <= row && row < 4);
        assert(0 <= col && col < 4);
        return m[row][col];
    }
    // 読み取り用
    float operator()(int row, int col) const
    {
        assert(0 <= row && row < 4);
        assert(0 <= col && col < 4);
        return m[row][col];
    }
    // *
    Matrix4 operator*(const Matrix4 &rhs) const
    {
        Matrix4 out;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    sum += (*this)(i, k) * rhs(k, j);
                }
                out(i, j) = sum;
            }
        }
        return out;
    }
    // *=
    Matrix4 &operator*=(const Matrix4 &rhs)
    {
        *this = *this * rhs;
        return *this;
    }
    // 単位行列生成用
    static Matrix4 Identity()
    {
        Matrix4 out;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                out(i, j) = (i == j) ? 1.0f : 0.0f;
            }
        }
        return out;
    }
    // 平行移動用変換行列を作成
    static Matrix4 Translate(const Vec3 &vec)
    {
        Matrix4 out = Identity();
        out(3, 0) = vec.x;
        out(3, 1) = vec.y;
        out(3, 2) = vec.z;
        return out;
    }
    // 拡大用変換行列の作成
    static Matrix4 Scale(const Vec3 &vec)
    {
        Matrix4 out = Identity();
        out(0, 0) = vec.x;
        out(1, 1) = vec.y;
        out(2, 2) = vec.z;
        return out;
    }
    // x軸回転
    static Matrix4 RotateX(float rad)
    {
        Matrix4 out = Identity();
        float cosA = cosf(rad);
        float sinA = sinf(rad);
        out(1, 1) = cosA;
        out(2, 1) = -sinA;
        out(1, 2) = sinA;
        out(2, 2) = cosA;
        return out;
    }
    // y軸回転
    static Matrix4 RotateY(float rad)
    {
        Matrix4 out = Identity();
        float cosA = cosf(rad);
        float sinA = sinf(rad);
        out(0, 0) = cosA;
        out(2, 0) = sinA;
        out(0, 2) = -sinA;
        out(2, 2) = cosA;
        return out;
    }
    // z軸回転
    static Matrix4 RotateZ(float rad)
    {
        Matrix4 out = Identity();
        float cosA = cosf(rad);
        float sinA = sinf(rad);
        out(0, 0) = cosA;
        out(1, 0) = -sinA;
        out(0, 1) = sinA;
        out(1, 1) = cosA;
        return out;
    }
    // 任意の軸(vec)を中心に回転させる
    static Matrix4 RotateAxis(const Vec3 &vec, float rad)
    {
        assert(vec.SqLength() > 0.0f);
        Vec3 axis = vec.GetNormalize();
        Matrix4 out = Identity();
        float cosA = cosf(rad);
        float sinA = sinf(rad);
        float oneMinusCos = 1.0f - cosA;
        out(0, 0) = axis.x * axis.x * oneMinusCos + cosA;
        out(1, 0) = axis.y * axis.x * oneMinusCos - axis.z * sinA;
        out(2, 0) = axis.z * axis.x * oneMinusCos + axis.y * sinA;
        out(0, 1) = axis.x * axis.y * oneMinusCos + axis.z * sinA;
        out(1, 1) = axis.y * axis.y * oneMinusCos + cosA;
        out(2, 1) = axis.z * axis.y * oneMinusCos - axis.x * sinA;
        out(0, 2) = axis.x * axis.z * oneMinusCos - axis.y * sinA;
        out(1, 2) = axis.y * axis.z * oneMinusCos + axis.x * sinA;
        out(2, 2) = axis.z * axis.z * oneMinusCos + cosA;
        return out;
    }
    // XYZ軸それぞれでの一括回転
    static Matrix4 RotateXYZ(const Vec3 &rad)
    {
        return RotateX(rad.x) * RotateY(rad.y) * RotateZ(rad.z);
    }
    // 拡大、回転、平行移動を一括で行う
    static Matrix4 SRT(const Vec3 &scale, const Vec3 &rad, const Vec3 &pos)
    {
        return Scale(scale) * RotateXYZ(rad) * Translate(pos);
    }
    // 行列を使った平行移動可能な座標変換
    Vec3 TransformPoint(const Vec3 &point) const
    {
        Vec3 out;
        out.x = point.x * (*this)(0, 0) + point.y * (*this)(1, 0) + point.z * (*this)(2, 0) + (*this)(3, 0);
        out.y = point.x * (*this)(1, 0) + point.y * (*this)(1, 1) + point.z * (*this)(1, 2) + (*this)(1, 3);
        out.z = point.x * (*this)(2, 0) + point.y * (*this)(2, 1) + point.z * (*this)(2, 2) + (*this)(2, 3);
        return out;
    }
    // 行列を使った平行移動不可能な回転拡大座標変換
    Vec3 TransformDirection(const Vec3 &direction) const
    {
        Vec3 out;
        out.x = direction.x * (*this)(0, 0) + direction.y * (*this)(1, 0) + direction.z * (*this)(2, 0);
        out.y = direction.x * (*this)(1, 0) + direction.y * (*this)(1, 1) + direction.z * (*this)(1, 2);
        out.z = direction.x * (*this)(2, 0) + direction.y * (*this)(2, 1) + direction.z * (*this)(2, 2);
        return out;
    }
};