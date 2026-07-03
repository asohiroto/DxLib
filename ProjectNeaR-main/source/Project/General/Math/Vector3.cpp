#include "Vector3.h"
#include "MyMath.h"
#include "Vector2.h"
#include "MathSub.h"
#include "Matrix4x4.h"
#include <random>
#include <cmath>
#include <cassert>

//3D用
Vector3::Vector3() :
    x(0.0f),
    y(0.0f),
    z(0.0f)
{
}

Vector3::Vector3(float initX, float initY, float initZ) :
    x(initX),
    y(initY),
    z(initZ)
{
}

Vector3::Vector3(VECTOR vec):
    x(vec.x),
    y(vec.y),
    z(vec.z)
{
}

float Vector3::Magnitude() const
{
    //三平方の定理で計算
    return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
}

float Vector3::SqMagnitude() const
{
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

Vector3 Vector3::Normalize() const
{
    if (this->Magnitude() <= 0.0f)
    {
        return *this;
    }
//    assert(this->Magnitude() > 0.0f && "正規化に失敗しました");
    //正規化
    Vector3 ans;
    ans = (*this) / Magnitude();
    return ans;
}

float Vector3::Dot(const Vector3& right) const
{
    //内積
    float ans;
    ans = this->x * right.x + this->y * right.y + this->z * right.z;
    return ans;
}

Vector3 Vector3::Cross(const Vector3& right) const
{
    //外積
    Vector3 ans;
    ans.x = this->y * right.z - this->z * right.y;
    ans.y = this->z * right.x - this->x * right.z;
    ans.z = this->x * right.y - this->y * right.x;
    return ans;
    //aとbのベクトルが作る平面に垂直でaからbに
    //右ねじを回したときにねじの進向きを持ち大きさは
    //aとbが作る平行四辺形の面積に等しい
}

DxLib::VECTOR Vector3::ToDxLibVector() const
{
    return VGet(this->x,this->y,this->z);
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float t)
{
	//線形補間
	if (t < 0.0f)t = 0.0f;
	if (t > 1.0f)t = 1.0f;
	Vector3 rV;
	rV = start * (1.0f - t) + end * t;
    return rV;
}

Vector2 Vector3::XZ() const
{
    Vector2 rV;
    rV.x = this->x;
    rV.y = this->z;
    return rV;
}

Vector2 Vector3::ZY()const
{
    Vector2 rV;
    rV.x = this->z;
    rV.y = this->y;
    return rV;
}

Vector2 Vector3::XY()const
{
    Vector2 rV;
    rV.x = this->x;
    rV.y = this->y;
    return rV;
}

Vector3 Vector3::operator+(const Vector3& right) const
{
    Vector3 ans;
    ans.x = this->x + right.x;
    ans.y = this->y + right.y;
    ans.z = this->z + right.z;
    return ans;
}

Vector3 Vector3::operator-(const Vector3& right) const
{
    Vector3 ans;
    ans.x = this->x - right.x;
    ans.y = this->y - right.y;
    ans.z = this->z - right.z;
    return ans;
}

Vector3 Vector3::operator*(const float& right) const
{
    Vector3 ans;
    ans.x = this->x * right;
    ans.y = this->y * right;
    ans.z = this->z * right;
    return ans;
}

Vector3 Vector3::operator/(const float& right) const
{
    Vector3 ans;
    ans.x = this->x / right;
    ans.y = this->y / right;
    ans.z = this->z / right;
    return ans;
}

Vector3 Vector3::operator=(const float& right) const
{
    Vector3 ans;
    ans.x = right;
    ans.y = right;
    ans.z = right;
    return ans;
}

Vector3 Vector3::operator+=(const Vector3& right)
{
    this->x = this->x + right.x;
    this->y = this->y + right.y;
    this->z = this->z + right.z;
    return *this;
}

Vector3 Vector3::operator-=(const Vector3& right)
{
    this->x = this->x - right.x;
    this->y = this->y - right.y;
    this->z = this->z - right.z;
    return *this;
}

Vector3 Vector3::operator*=(const float& right)
{
    this->x = this->x * right;
    this->y = this->y * right;
    this->z = this->z * right;
    return *this;
}

Vector3 Vector3::operator/=(const float& right)
{
    this->x = this->x / right;
    this->y = this->y / right;
    this->z = this->z / right;
    return *this;
}

bool Vector3::operator==(const Vector3& right) const
{
    bool isFlag = (this->x == right.x && this->y == right.y && this->z == right.z);
    return isFlag;
}

bool Vector3::operator!=(const Vector3& right) const
{
    bool isFlag = (this->x != right.x || this->y != right.y || this->z != right.z);
    return isFlag;
}

Vector3 Vector3::Zero()
{
    Vector3 zero;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    return zero;
}

Vector3 Vector3::GetRandVecXZ()
{
	Vector3 randVec;
	//乱数を生成して角度を計算
    float angle = MyMath::GetRandF(0.0f, 2.0f * MyMath::PI_F);
	//X軸とZ軸の成分を計算
	randVec.x = std::cosf(angle);
	randVec.z = std::sinf(angle);
    //正規化
    if(randVec.SqMagnitude() > 0.0f)
    {
        randVec = randVec.Normalize();
	}
    return randVec;
}

