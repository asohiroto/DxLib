#pragma once
#include "Vec3.h"
#include <DxLib.h>
#include <cassert>
#include <cmath>
#include <iterator>

class Matrix4
{
public:
	float m[4][4] = {};

public:
	// 書き込み用
	float& operator()(int row, int col)
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
	Matrix4 operator*(const Matrix4& rhs) const
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
	Matrix4& operator*=(const Matrix4& rhs)
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
	static Matrix4 Translate(const Vec3& vec)
	{
		Matrix4 out = Identity();
		out.SetPosition(vec);
		return out;
	}
	// 拡大用変換行列の作成
	static Matrix4 Scale(const Vec3& vec)
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
	static Matrix4 RotateAxis(const Vec3& vec, float rad)
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
	static Matrix4 RotateXYZ(const Vec3& rad)
	{
		return RotateX(rad.x) * RotateY(rad.y) * RotateZ(rad.z);
	}
	// 拡大、回転、平行移動を一括で行う
	static Matrix4 SRT(const Vec3& scale, const Vec3& rad, const Vec3& pos)
	{
		return Scale(scale) * RotateXYZ(rad) * Translate(pos);
	}
	// アフィン変換であるかの確認
	void AssertAffine() const
	{
		const Matrix4& a = *this;
		assert(a(0, 3) == 0.0f);
		assert(a(1, 3) == 0.0f);
		assert(a(2, 3) == 0.0f);
		assert(a(3, 3) == 1.0f);
	}
	// 行列を使った平行移動可能な座標変換(アフィン変換専用)
	Vec3 TransformPoint(const Vec3& point) const
	{
		AssertAffine();
		return TransformDirection(point) + GetPosition();
	}
	// 行列を使った平行移動不可能な回転拡大座標変換
	Vec3 TransformDirection(const Vec3& direction) const
	{
		return GetRight() * direction.x + GetUp() * direction.y + GetForward() * direction.z;
	}
	// 射影行列にも対応した座標変換
	Vec3 TransformCoord(const Vec3& point) const
	{
		const Matrix4& a = *this;
		Vec3 v = TransformDirection(point) + GetPosition();
		float w = point.x * a(0, 3) + point.y * a(1, 3) + point.z * a(2, 3) + a(3, 3);
		assert(fabsf(w) > 1e-6f);
		return Vec3(v / w);
	}
	// 非一様スケールを含まない行列の法線変換
	Vec3 TransformNormal(const Vec3& normal) const
	{
		Vec3 n = TransformDirection(normal);
		n.Normalize();
		return n;
	}
	// DxLIb内MATRIX型に変換
	MATRIX ToMAT() const
	{
		MATRIX out;
		const Matrix4& a = *this;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				out.m[i][j] = a(i, j);
			}
		}
		return out;
	}
	// DxLib内MATRIX型をMatrix4に変換
	static Matrix4 ToMat(const MATRIX& mat)
	{
		Matrix4 out;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				out(i, j) = mat.m[i][j];
			}
		}
		return out;
	}
	// アフィン変換行列の逆行列
	Matrix4 InverseAffine() const
	{
		AssertAffine();
		Vec3 r0 = GetRight();
		Vec3 r1 = GetUp();
		Vec3 r2 = GetForward();
		Vec3 c0 = r1.Cross(r2);
		Vec3 c1 = r2.Cross(r0);
		Vec3 c2 = r0.Cross(r1);
		float det = r0.Dot(c0);
		assert(fabsf(det) > 1e-8f);
		float invDet = 1.0f / det;
		Matrix4 out = Identity();
		out(0, 0) = c0.x * invDet;
		out(0, 1) = c1.x * invDet;
		out(0, 2) = c2.x * invDet;
		out(1, 0) = c0.y * invDet;
		out(1, 1) = c1.y * invDet;
		out(1, 2) = c2.y * invDet;
		out(2, 0) = c0.z * invDet;
		out(2, 1) = c1.z * invDet;
		out(2, 2) = c2.z * invDet;
		Vec3 t = GetPosition();
		out.SetPosition(out.TransformDirection(-t));
		return out;
	}
	// 転置行列
	Matrix4 Transposed() const
	{
		Matrix4 out;
		const Matrix4& a = *this;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				out(j, i) = a(i, j);
			}
		}
		return out;
	}
	// 行列式の計算
	float Determinant() const
	{
		AssertAffine();
		return GetRight().Dot(GetUp().Cross(GetForward()));
	}
	// 平行移動成分を取り出す
	Vec3 GetPosition() const
	{
		const Matrix4& a = *this;
		return Vec3(a(3, 0), a(3, 1), a(3, 2));
	}
	// 平行移動成分の書き換え
	void SetPosition(const Vec3& pos)
	{
		Matrix4& a = *this;
		a(3, 0) = pos.x;
		a(3, 1) = pos.y;
		a(3, 2) = pos.z;
	}
	// この行列が表すローカルのX軸(右方向)を取り出す
	Vec3 GetRight() const
	{
		const Matrix4& a = *this;
		return Vec3(a(0, 0), a(0, 1), a(0, 2));
	}
	// この行列が表すローカルのY軸(上方向)を取り出す
	Vec3 GetUp() const
	{
		const Matrix4& a = *this;
		return Vec3(a(1, 0), a(1, 1), a(1, 2));
	}
	// この行列があらわすローカルのZ軸（前方向）を取り出す
	Vec3 GetForward() const
	{
		const Matrix4& a = *this;
		return Vec3(a(2, 0), a(2, 1), a(2, 2));
	}
	// ローカルのX軸を定める
	void SetRight(const Vec3& axis)
	{
		Matrix4& a = *this;
		a(0, 0) = axis.x;
		a(0, 1) = axis.y;
		a(0, 2) = axis.z;
	}
	// ローカルのY軸を定める
	void SetUp(const Vec3& axis)
	{
		Matrix4& a = *this;
		a(1, 0) = axis.x;
		a(1, 1) = axis.y;
		a(1, 2) = axis.z;
	}
	// ローカルのZ軸を定める
	void SetForward(const Vec3& axis)
	{
		Matrix4& a = *this;
		a(2, 0) = axis.x;
		a(2, 1) = axis.y;
		a(2, 2) = axis.z;
	}
	// 各軸の大きさを取り出す
	Vec3 GetScale() const
	{
		return Vec3(GetRight().Length(), GetUp().Length(), GetForward().Length());
	}
	// ビュー行列を作る。eye の位置から target を見るカメラを表す。
	// up はカメラの上方向(通常は Vec3(0, 1, 0))。
	// SetCameraViewMatrix に渡して使う。
	static Matrix4 LookAtLH(const Vec3& eye, const Vec3& target, const Vec3& up)
	{
		Matrix4 out = LookRotation(target - eye, up);
		out.SetPosition(eye);
		return out.InverseAffine();
	}
	// forwardの方向を向く回転行列を作る
	static Matrix4 LookRotation(const Vec3& forward, const Vec3& up)
	{
		assert(forward.SqLength() > 0.0f);
		assert(up.SqLength() > 0.0f);
		Vec3 zAxis = forward.GetNormalize();
		Vec3 xAxis = up.Cross(zAxis);
		assert(xAxis.SqLength() > 0.0f);
		xAxis.Normalize();
		Vec3 yAxis = zAxis.Cross(xAxis);
		Matrix4 out = Identity();
		out.SetRight(xAxis);
		out.SetUp(yAxis);
		out.SetForward(zAxis);
		return out;
	}
	// 透視投影行列を作る。遠くの物ほど小さく見える、普通の 3D 表示用。
	// fovY:縦の視野角、aspect:画面の横/縦、zNear/zFar:描画する奥行きの範囲
	static Matrix4 PerspectiveFovLH(float fovY, float aspect, float zNear, float zFar)
	{
		assert(zFar > zNear);
		assert(zNear > 0.0f);
		assert(aspect > 0.0f);
		assert(fovY > 0.0f);
		assert(fovY < DX_PI_F);
		float yScale = 1.0f / tanf(fovY / 2.0f);
		float xScale = yScale / aspect;
		Matrix4 out;
		out(0, 0) = xScale;
		out(1, 1) = yScale;
		out(2, 2) = zFar / (zFar - zNear);
		out(2, 3) = 1.0f;
		out(3, 2) = -zFar * zNear / (zFar - zNear);
		return out;
	}
	// 正射影行列を作る。距離で大きさが変わらない表示用(2D 風表示やミニマップなど)。
	static Matrix4 OrthoLH(float width, float height, float zNear, float zFar)
	{
		assert(width > 0.0f);
		assert(height > 0.0f);
		assert(zFar > zNear);
		Matrix4 out = Identity();
		out(0, 0) = 2.0f / width;
		out(1, 1) = 2.0f / height;
		out(2, 2) = 1.0f / (zFar - zNear);
		out(3, 2) = -zNear / (zFar - zNear);
		return out;
	}
	// 全要素が完全に一致するか比較する。
	// float の誤差があるので、計算結果の比較には NearlyEquals を使うほうが安全。
	bool operator==(const Matrix4& rhs) const
	{
		const Matrix4& a = *this;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (a(i, j) != rhs(i, j))
					return false;
			}
		}
		return true;
	}
	// いずれかの要素が不一致かを返す
	bool operator!=(const Matrix4& rhs) const
	{
		const Matrix4& a = *this;
		if (a == rhs)
			return false;
		else
			return true;
	}
	// 全要素の差が eps 以内なら等しいとみなす。テストやデバッグで使う。
	bool NearlyEquals(const Matrix4& rhs, float eps = 1e-5f) const
	{
		const Matrix4& a = *this;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (fabsf(a(i, j) - rhs(i, j)) > eps)
					return false;
			}
		}
		return true;
	}
};