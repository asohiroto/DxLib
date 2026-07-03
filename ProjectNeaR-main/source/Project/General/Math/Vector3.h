#pragma once
#include <DxLib.h>

class Vector2;
class Vector3
{
public:
	Vector3();
	Vector3(float initX, float initY, float initZ);
	Vector3(VECTOR vec);
	~Vector3() {};
	float x, y, z;

	/// <summary>
	/// ベクトルの大きさを返します
	/// </summary>
	/// <returns></returns>
	float Magnitude()const;

	/// <summary>
	/// ベクトルの大きさの2乗を返します
	/// </summary>
	/// <returns></returns>
	float SqMagnitude() const;

	/// <summary>
	/// 正規化(大きさを1にする)
	/// </summary>
	Vector3 Normalize() const;

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="right"></param>
	/// <returns></returns>
	float Dot(const Vector3& right)const;

	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="right"></param>
	/// <returns></returns>
	Vector3 Cross(const Vector3& right)const;

	/// <summary>
	/// DxLibのベクトルに変換します
	/// </summary>
	/// <returns></returns>
	VECTOR ToDxLibVector()const;

	/// <summary>
	/// Lerp(線形補間)を行います。
	/// </summary>
	/// <param name="start">開始位置(座標)</param>
	/// <param name="end">終了位置(座標)</param>
	/// <param name="t">補間の値</param>
	/// <returns>補間後のベクトル(座標)</returns>
	static Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

	/// <summary>
	/// XZの成分を持つべく２を返す
	/// </summary>
	/// <returns></returns>
	Vector2 XZ()const;

	/// <summary>
	/// ZYの成分を持つべく２を返す
	/// </summary>
	/// <returns></returns>
	Vector2 ZY()const;

	/// <summary>
	/// XYの成分を持つべく２を返す
	/// </summary>
	/// <returns></returns>
	Vector2 XY()const;
	
	/// <summary>
	/// Slerp(球面線形補間)を行います。
	/// </summary>
	/// <param name="start">開始位置(座標)</param>
	/// <param name="end">終了位置(座標)</param>
	/// <param name="t">補間の値</param>
	/// <returns>補間後のベクトル(座標)</returns>
	//static Vector3 Slerp(const Vector3& start, const Vector3& end, float t);

	//演算子オーバーロード
	Vector3 operator+(const Vector3& right)const;
	Vector3 operator-(const Vector3& right)const;
	Vector3 operator*(const float& right)const;
	Vector3 operator/(const float& right)const;
	Vector3 operator=(const float& right)const;
	Vector3 operator+=(const Vector3& right);
	Vector3 operator-=(const Vector3& right);
	Vector3 operator*=(const float& right);
	Vector3 operator/=(const float& right);
	bool operator==(const Vector3& right)const;
	bool operator!=(const Vector3& right)const;

	//基底ベクトル
	static Vector3 Up() { return Vector3(0.0f, 1.0f, 0.0f); };
	static Vector3 Down() { return Vector3(0.0f, -1.0f, 0.0f); };
	static Vector3 Left() { return Vector3(-1.0f, 0.0f, 0.0f); };
	static Vector3 Right() { return Vector3(1.0f, 0.0f, 0.0f); };
	static Vector3 Forward() { return Vector3(0.0f, 0.0f, 1.0f); };
	static Vector3 Back() { return Vector3(0.0f, 0.0f, -1.0f); };
	/// <summary>
	/// 0ベクトルにする
	/// </summary>
	static Vector3 Zero();
	
	/// <summary>
	/// ランダムなベクトルを生成します(XZ)
	/// </summary>
	/// <returns></returns>
	static Vector3 GetRandVecXZ();
};
//座標で表すときに使う
typedef Vector3 Position3;
