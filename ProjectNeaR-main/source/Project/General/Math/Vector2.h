#pragma once
#include <cmath>
class Vector3;
class Vector2
{
public:
	/// <summary>
	/// 2D用
	/// </summary>
	Vector2() :x(0.0f), y(0.0f) {};
	Vector2(float initX, float initY) :x(initX), y(initY) {};
	float x, y;
	/// <summary>
	/// ベクトルの大きさを返します
	/// </summary>
	/// <returns></returns>
	float Magnitude()const;

	/// <summary>
	/// 正規化(大きさを1にする)
	/// </summary>
	Vector2 Normalize() const;

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="right"></param>
	/// <returns></returns>
	float Dot(const Vector2& right)const;

	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="right"></param>
	/// <returns>スカラー</returns>
	float Cross(const Vector2& right)const;

	/// <summary>
	/// 0ベクトルにする
	/// </summary>
	Vector2 Zero()const;

	//演算子オーバーロード
	Vector2 operator+(const Vector2& right)const;
	Vector2 operator-(const Vector2& right)const;
	Vector2 operator*(const float& right)const;
	Vector2 operator/(const float& right)const;
	Vector2 operator+=(const Vector2& right);
	Vector2 operator-=(const Vector2& right);
	Vector2 operator*=(const float& right);
	Vector2 operator/=(const float& right);
	bool operator==(const Vector2& right)const;
	bool operator!=(const Vector2& right)const;

	static float GetRad(Vector2 vec1, Vector2 vec2);

	Vector3 XZ();

	//線形補間
	static Vector2 Lerp(Vector2 v1, Vector2 v2, float t);

	//矩形範囲内にあるかをチェック
	static bool IsPointInRect(const Vector2& A, const Vector2& B, const Vector2& C);
};

//座標で表すときに使う
typedef Vector2 Position2;

