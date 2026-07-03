#include "Vector2.h"
#include "Vector3.h"
#include "MathSub.h"
#include <DxLib.h>
#include <cassert>

//2D用
float Vector2::Magnitude() const
{
    //三平方の定理で計算
    return sqrtf(this->x * this->x + this->y * this->y);
}

Vector2 Vector2::Normalize()const
{
    //正規化した値を返す
    Vector2 ans = *this / Magnitude();
    return ans;
}

float Vector2::Dot(const Vector2& right) const
{
    float ans;
    ans = this->x * right.x + this->y * right.y;
    return ans;
}

float Vector2::Cross(const Vector2& right) const
{
    //外積(スカラー)
    float ans;
    ans = this->x * right.y - this->y * right.x;
    return ans;
    //aとbのベクトルが作る平面に垂直でaからbに
    //右ねじを回したときにねじの進向きを持ち大きさは
    //aとbが作る平行四辺形の面積に等しい
}

Vector2 Vector2::Zero() const
{
    Vector2 zaro{ 0.0f ,0.0f };
    return zaro;
}

Vector2 Vector2::operator+(const Vector2& right) const
{
    Vector2 ans;
    ans.x = this->x + right.x;
    ans.y = this->y + right.y;
    return ans;
}

Vector2 Vector2::operator-(const Vector2& right) const
{
    Vector2 ans;
    ans.x = this->x - right.x;
    ans.y = this->y - right.y;
    return ans;
}

Vector2 Vector2::operator*(const float& right) const
{
    Vector2 ans;
    ans.x = this->x * right;
    ans.y = this->y * right;
    return ans;
}

Vector2 Vector2::operator/(const float& right) const
{
    Vector2 ans;
    ans.x = this->x / right;
    ans.y = this->y / right;
    return ans;
}

Vector2 Vector2::operator+=(const Vector2& right)
{
    this->x += right.x;
    this->y += right.y;
    return *this;
}

Vector2 Vector2::operator-=(const Vector2& right)
{
    this->x -= right.x;
    this->y -= right.y;
    return *this;
}

Vector2 Vector2::operator*=(const float& right)
{
    this->x *= right;
    this->y *= right;
    return *this;
}

Vector2 Vector2::operator/=(const float& right)
{
    //0を以外なら計算する
    assert(right != 0.0f);
    this->x /= right;
    this->y /= right;
    return *this;
}

bool Vector2::operator==(const Vector2& right) const
{
    bool isX = this->x == right.x;
    bool isY = this->y == right.y;
    return isX && isY;
}

bool Vector2::operator!=(const Vector2& right) const
{
    bool isX = this->x != right.x;
    bool isY = this->y != right.y;
    return isX || isY;
}

float Vector2::GetRad(Vector2 vec1, Vector2 vec2)
{
    //ベクトルがない
    if (vec1.Magnitude() <= 0.0f || vec2.Magnitude() <= 0.0f)
    {
        return 0.0f;
    }

    //正規化
    Vector2 v1 = vec1.Normalize();
    Vector2 v2 = vec2.Normalize();

    //内積から間の角度を取得
    float dot = v1.Dot(v2);
    //クランプ
    dot = MathSub::ClampFloat(dot, -1.0f, 1.0f);
    //Θ(角度)を取得
    float rad = acosf(dot);

	return rad;
}

Vector3 Vector2::XZ()
{
    Vector3 rV;
    rV.x = this->x;
    rV.y = 0.0f;
    rV.z = this->y;
    return rV;
}

Vector2 Vector2::Lerp(Vector2 v1, Vector2 v2, float t)
{
    //線形補間
    if (t < 0.0f)t = 0.0f;
    if (t > 1.0f)t = 1.0f;
    Vector2 rV;
    rV = v1 * (1.0f - t) + v2 * t;
    return rV;
}

bool Vector2::IsPointInRect(const Vector2& A, const Vector2& B, const Vector2& C)
{
    float left = MathSub::Min(A.x, B.x);
    float right = MathSub::Max(A.x, B.x);
    float top = MathSub::Min(A.y, B.y);
    float bottom = MathSub::Max(A.y, B.y);

    return (C.x >= left && C.x <= right &&
        C.y >= top && C.y <= bottom);
}
