#pragma once
#include<cmath>

class Vec2
{
public:
	float x;
	float y;

	Vec2() :
		x(0.0f), y(0.0f)
	{

	}

	Vec2(float posX, float posY) :
		x(posX), y(posY)
	{

	}

	// 単項演算子+
	Vec2 operator+() const
	{
		return *this;
	}

	// 単項演算子-
	Vec2 operator-() const
	{
		return Vec2{ -x, -y };
	}

	// 足し算
	Vec2 operator+(Vec2 vec) const
	{
		return Vec2{ x + vec.x, y + vec.y };
	}

	// Vec2 += Vec2
	Vec2 operator+=(Vec2 vec)
	{
		x += vec.x;
		y += vec.y;
		return *this;

	}

	// 引き算
	Vec2 operator-(Vec2 vec) const
	{
		return Vec2{ x - vec.x, y - vec.y };
	}

	// Vec2 -= Vec2
	Vec2 operator-=(Vec2 vec)
	{
		x -= vec.x;
		y -= vec.y;
		return *this;
	}

	// 掛け算
	Vec2 operator*(float scale) const
	{
		return Vec2{ x * scale, y * scale };
	}

	// Vec2 *= float
	Vec2 operator*=(float scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	// 割り算
	Vec2 operator/(float scale) const
	{
		return Vec2{ x / scale, y / scale };
	}

	// Vec2 /= float
	Vec2 operator/=(float scale)
	{
		x /= scale;
		y /= scale;
		return *this;
	}

	bool operator==(Vec2 vec)
	{
		if (x == vec.x && y == vec.y) return true;
		else return false;
	}

	// ベクトルの長さの二乗を求める
	float sqLength() const
	{
		return x * x + y * y;
	}

	// ベクトルの長さを求める
	float length() const
	{
		return sqrtf(sqLength());
	}

	// 自身の正規化を行う
	void normalize()
	{
		float len = length();
		if (len > 0.0f)
		{
			x /= len;
			y /= len;
		}
		// 長さが０の場合は何もしない
	}

	// 自身を正規化したベクトルを取得する
	Vec2 getNormalize() const
	{
		float len = length();
		if (len > 0.0f)
		{
			return Vec2{ x / len, y / len };
		}
		// 長さが０の場合
		return Vec2{ 0.0f, 0.0f };
	}

	// ベクトルをゼロに
	static Vec2 Zero()
	{
		return Vec2(0, 0);
	}

	// 画面上向きのベクトルに
	static Vec2 Up()
	{
		return Vec2(0, -1);
	}

	// 画面下向きのベクトルに
	static Vec2 Down()
	{
		return Vec2(0, 1);
	}

	// 画面右向きのベクトルに
	static Vec2 Right()
	{
		return Vec2(1, 0);
	}

	// 画面左向きのベクトルに
	static Vec2 Left()
	{
		return Vec2(-1, 0);
	}

	// 内積を求める
	float Dot(const Vec2& other) const
	{
		return x * other.x + y * other.y;
	}

	// 外積を求める
	// 正ならvec2が左側
	float Cross(const Vec2& other) const
	{
		return x * other.y - y * other.x;
	}

	/// <summary>
	/// 目的地へのの単位ベクトルを返す
	/// </summary>
	/// <param name="point1">終点</param>
	/// <returns></returns>
	Vec2 ToTargetNol(const Vec2& point1)
	{
		Vec2 target = point1 - Vec2(x, y);
		target.normalize();

		return target;
	}

	/// <summary>
	/// 線形補間を行う
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="goal">終点</param>
	/// <param name="t">率</param>
	/// <returns></returns>
	static Vec2 Lerp(const Vec2& start, const Vec2& goal, float t)
	{
		if (t <= 0) t = 0;
		if (t >= 1) t = 1;

		return start + (goal - start) * t;
	}

	// 対象との距離を返す
	float DistanceDouble(const Vec2& target) const
	{
		return (x * target.x + y * target.y);
	}
};