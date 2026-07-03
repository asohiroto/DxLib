#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "MathSub.h"

namespace MyMath
{
	constexpr double PI = 3.14159265358979323846;
	constexpr double TwoPI = 3.14159265358979323846 * 2.0;
	constexpr float PI_F = 3.14159265358979323846f;
	constexpr float TwoPI_F = 3.14159265358979323846f * 2.0f;


	// 度数法を弧度法に変更
	constexpr float DEG_2_RAD = PI_F / 180.0f;
	// 弧度法を度数法に変更
	constexpr float RAD_2_DEG = 180.0f / PI_F;

	/// <summary>
	/// ランダムな値を生成します。
	/// min以上max未満の値を返します。
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns>float型</returns>
	float GetRandF(float min, float max);
	/// <summary>
	/// ランダムな値を生成します。
	/// min以上max以下の値を返します。
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns>int型</returns>
	int GetRand(int min, int max);
	/// <summary>
	/// ランダムでオンオフを返す
	/// </summary>
	/// <returns></returns>
	bool IsRand();
}

