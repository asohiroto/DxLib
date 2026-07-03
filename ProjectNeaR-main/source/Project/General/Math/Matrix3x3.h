#pragma once
#include "Vector2.h"
#include <cmath>
class Matrix3x3
{
public:
	Matrix3x3();
	~Matrix3x3() {};

	float mat[3][3];
	/// <summary>
	/// ２つの行列の乗算を返す(乗算の順番に注意)
	/// </summary>
	/// <param name="left">左辺行列</param>
	/// <param name="right">右辺行列</param>
	/// <returns></returns>
	Matrix3x3  MultipleMat3x3(const Matrix3x3& left, const Matrix3x3& right);
	Matrix3x3 operator* (const Matrix3x3& right);

	/// <summary>
	/// ベクトルに対して行列乗算を適用し、結果のベクトルを返す
	/// </summary>
	/// <param name="m">行列</param>
	/// <param name="v">ベクトル</param>
	/// <returns></returns>
	Vector2 MultipleVec2(const Matrix3x3& m, const Vector2& v);
	Vector2 operator*(const Vector2& v);

};

/// <summary>
/// /単位行列を返す(3x3)
/// </summary>
/// <returns></returns>
Matrix3x3 IdentityMat3x3();

/// <summary>
/// 平行移動行列を返す(3x3)
/// </summary>
/// <param name="x">X方向の移動量</param>
/// <param name="y">Y方向の移動量</param>
/// <returns></returns>
Matrix3x3 TranslateMat3x3(float x, float y);

/// <summary>
/// 回転行列を返す(3x3)
/// </summary>
/// <param name="angle">回転角度</param>
/// <returns></returns>
Matrix3x3 RotateMat3x3(float angle);

/// <summary>
/// とある点を中心に回転して、その回転後の座標を返します
/// </summary>
/// <param name="center">回転中心点</param>
/// <param name="angle">回転角度</param>
/// <returns></returns>
Matrix3x3 RotatePositionMatrix3x3(const Vector2& center, float angle);

