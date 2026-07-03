#pragma once
#include "Vector3.h"
class Matrix4x4
{
public:
	Matrix4x4();
	~Matrix4x4() {};

	float mat[4][4];

	/// <summary>
	/// ２つの行列の乗算を返す(乗算の順番に注意)
	/// </summary>
	/// <param name="left">左辺行列</param>
	/// <param name="right">右辺行列</param>
	/// <returns></returns>
	Matrix4x4 MultipleMat4x4(const Matrix4x4& left, const Matrix4x4& right);
	Matrix4x4 operator* (const Matrix4x4& right);

	/// <summary>
	/// ベクトルに対して行列乗算を適用し、結果のベクトルを返す
	/// </summary>
	/// <param name="m">行列</param>
	/// <param name="v">ベクトル</param>
	/// <returns></returns>
	Vector3 MultipleVec3(const Matrix4x4& m, const Vector3& v);
	Vector3 operator*(const Vector3& v);

	/// <summary>
	/// /単位行列を返す(4x4)
	/// </summary>
	/// <returns></returns>
	static Matrix4x4 IdentityMat4x4();
	
	/// <summary>
	/// 平行移動行列を返す(4x4)
	/// </summary>
	/// <param name="x">X方向の移動量</param>
	/// <param name="y">Y方向の移動量</param>
	/// <param name="z">Z方向の移動量</param>
	/// <returns></returns>
	static Matrix4x4 TranslateMat4x4(float x, float y, float z);

	/// <summary>
	/// X軸回転行列を返す(4x4)
	/// </summary>
	/// <param name="angle">回転角度</param>
	/// <returns></returns>
	static Matrix4x4 RotateXMat4x4(float angle);

	/// <summary>
	/// Y軸回転行列を返す(4x4)
	/// </summary>
	/// <param name="angle">回転角度</param>
	/// <returns></returns>
	static Matrix4x4 RotateYMat4x4(float angle);

	/// <summary>
	/// Z軸回転行列を返す(4x4)
	/// </summary>
	/// <param name="angle">回転角度</param>
	/// <returns></returns>
	static Matrix4x4 RotateZMat4x4(float angle);


	/// <summary>
	/// とある点を中心にY軸回転して、その回転後の座標を返します
	/// </summary>
	/// <param name="center">回転中心点</param>
	/// <param name="angle">回転角度</param>
	/// <returns></returns>
	static Matrix4x4 RotateYPositionMatrix4x4(const Position3& center, float angle);

	/// <summary>
	/// とある点を中心にX軸回転して、その回転後の座標を返します
	/// </summary>
	/// <param name="center">回転中心点</param>
	/// <param name="angle">回転角度</param>
	/// <returns></returns>
	static Matrix4x4 RotateXPositionMatrix4x4(const Position3& center, float angle);

	/// <summary>
	/// とある点を中心にZ軸回転して、その回転後の座標を返します
	/// </summary>
	/// <param name="center">回転中心点</param>
	/// <param name="angle">回転角度</param>
	/// <returns></returns>
	static Matrix4x4 RotateZPositionMatrix4x4(const Position3& center, float angle);

	/// <summary>
	/// とある点を中心にX軸回転してY軸回転して、その回転後の座標を返します
	/// </summary>
	/// <param name="center">回転中心点</param>
	/// <param name="hAngle">回転角度</param>
	/// <param name="vAngle">回転角度</param>
	/// <returns></returns>
	static Matrix4x4 RotateXYPositionMatrix4x4(const Position3& center, float hAngle, float vAngle);

	/// <summary>
	/// 大きさの行列
	/// </summary>
	/// <param name="scele"></param>
	/// <returns></returns>
	static Matrix4x4 ScaleMatrix4x4(const Vector3& scele);

	/// <summary>
	/// DXライブラリの行列に変換
	/// </summary>
	/// <returns></returns>
	MATRIX ToDxLibMATRIX();

	/// <summary>
	/// DXライブラリの行列に変換
	/// </summary>
	/// <returns></returns>
	static Matrix4x4 ToMatrix4x4(MATRIX dxMat);

	/// <summary>
	/// 向きたい方向を向く回転行列を返す
	/// </summary>
	/// <param name="target">向きたい方向</param>
	/// <param name="up">床が傾いている時に値を渡す</param>
	/// <returns>回転行列</returns>
	static Matrix4x4 LookAt(const Vector3& target,const Vector3 up = Vector3::Up());
};
