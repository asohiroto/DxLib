#include "Quaternion.h"
#include <cmath>
#include <cassert>
Quaternion::Quaternion():
	w(1.0f),
	x(0.0f),
	y(0.0f),
	z(0.0f)
{
}

Quaternion::Quaternion(float w,float x, float y, float z):
	w(w),
	x(x),
	y(y),
	z(z)
{
}
Quaternion::Quaternion(const Quaternion& q) :
	w(q.w),
	x(q.x),
	y(q.y),
	z(q.z)
{
}
//クォータニオンの回転の合成
Quaternion Quaternion::operator*(const Quaternion& q)const
{
	//クォータニオンの掛け算
	Quaternion rQ;
	rQ.w = this->w * q.w - this->x * q.x - this->y * q.y - this->z * q.z;//実部
	rQ.x = this->w * q.x + this->x * q.w + this->y * q.z - this->z * q.y;//虚部
	rQ.y = this->w * q.y + this->y * q.w + this->z * q.x - this->x * q.z;//虚部
	rQ.z = this->w * q.z + this->z * q.w + this->x * q.y - this->y * q.x;//虚部
	return rQ;
}
//ベクトル回転させる
Vector3 Quaternion::operator*(const Vector3& v)const
{
	Quaternion qPos, qInv;
	Vector3 vPos;
	//三次元座標をクォータニオンに変換
	qPos.w = 1.0f;
	qPos.x = v.x;
	qPos.y = v.y;
	qPos.z = v.z;

	//回転クォータニオンのインバースを作成
	qInv = this->InverseQ();
	//回転クォータニオンの作成
	qPos = *this * qPos * qInv;
	//三次元座標に戻す
	vPos.x = qPos.x;
	vPos.y = qPos.y;
	vPos.z = qPos.z;

	return vPos;
}
Quaternion Quaternion::operator*(const float& s)const
{
	Quaternion rQ;
	rQ.w = this->w * s;
	rQ.x = this->x * s;
	rQ.y = this->y * s;
	rQ.z = this->z * s;
	return rQ;
}
Quaternion Quaternion::operator+(const Quaternion& q) const
{
	Quaternion rQ;
	rQ.w = this->w + q.w;
	rQ.x = this->x + q.x;
	rQ.y = this->y + q.y;
	rQ.z = this->z + q.z;
	return rQ;
}
bool Quaternion::operator==(const Quaternion& q) const
{
	return (this->w == q.w && this->x == q.x && this->y == q.y && this->z == q.z);
}
bool Quaternion::operator!=(const Quaternion& q) const
{
	return !(*this == q);
}
//二つのクォータニオンの間の角度を求める際に使う
float Quaternion::DotQ(const Quaternion& q)const
{
	float result = this->w * q.w + this->x * q.x + this->y * q.y + this->z * q.z;
	return result;
}

//正規化クォータニオン(値の誤差をなくす)
Quaternion Quaternion::NormQ()const
{
	//単位クォータニオン
	Quaternion rQ = IdentityQ();
	//軸があるかチェック
	if (this->AxisMagnitude() <= 0.0f)
	{
		return rQ;//回転しない
	}
	//自分の大きさを出す(絶対値)
	float length = this->Magnitude();
	//自分に自分のインバースをかけて正規化
	//rQ = *this * this->InverseQ();
	rQ.w = this->w / length;
	rQ.x = this->x / length;
	rQ.y = this->y / length;
	rQ.z = this->z / length;
	return rQ;
}

//回転しないクォータニオン(単位クォータニオン)
Quaternion Quaternion::IdentityQ()
{
	return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
}

//共役(逆)クォータニオン
Quaternion Quaternion::InverseQ()const
{
	return  Quaternion(this->w, -this->x, -this->y, -this->z);
}

float Quaternion::Magnitude() const
{
	float magnitude = sqrt((this->w * this->w) + (this->x * this->x) + (this->y * this->y) + (this->z * this->z));
	return magnitude;
}


float Quaternion::AxisMagnitude() const
{
	float magnitude = sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
	return magnitude;
}

Quaternion Quaternion::Euler(float xDeg, float yDeg, float zDeg)
{
	//ラジアンに変換
	float xRad = xDeg * MyMath::DEG_2_RAD;
	float yRad = yDeg * MyMath::DEG_2_RAD;
	float zRad = zDeg * MyMath::DEG_2_RAD;

	//単位クォータニオン
	Quaternion rQ = IdentityQ();
	//各要素の回転クォータニオンを作成
	Vector3 right = { 1.0f,0.0f,0.0f };
	Quaternion xQ = AngleAxis(xRad, right);//ロール
	Vector3 up = { 0.0f,1.0f,0.0f };
	Quaternion yQ = AngleAxis(yRad, up);//ヨー
	Vector3 forward = { 0.0f,0.0f,-1.0f };
	Quaternion zQ = AngleAxis(zRad, forward);//ピッチ
	//計算
	rQ = yQ * xQ * zQ;//Unityの回転順序に合わせる
	return rQ.NormQ();
}

Vector3 Quaternion::GetEuler()
{
	Vector3 euler;

	//正規化安全のため）
	Quaternion q = this->NormQ();

	//ロールX軸の回転）
	float numeX = 2.0f * (q.w * q.x + q.y * q.z);
	float denoX = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	numeX = MathSub::ClampFloat(numeX, -1.0f, 1.0f);
	denoX = MathSub::ClampFloat(denoX, -1.0f, 1.0f);
	euler.x = std::atan2(numeX, denoX);

	//ピッチ(Y軸の回転）
	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1.0f)//誤差があるなら
	{
		sinp = MathSub::ClampFloat(sinp, -1.0f, 1.0f);
	}
	euler.y = std::asin(sinp);

	//ヨー(Z軸の回転）
	float numeZ = 2.0f * (q.w * q.z + q.x * q.y);
	float denoZ = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	numeZ = MathSub::ClampFloat(numeZ, -1.0f, 1.0f);
	denoZ = MathSub::ClampFloat(denoZ, -1.0f, 1.0f);
	euler.z = std::atan2(numeZ, denoZ);

	return euler;
}
Matrix4x4 Quaternion::GetMatrix() const
{
	Matrix4x4 rM = Matrix4x4::IdentityMat4x4();
	Quaternion q = *this;
	if (q.Magnitude() > 0.0f)
	{
		q = q.NormQ();
	}
	// クォータニオンの各成分を使って行列の要素を計算
	auto xy2 = q.x * q.y * 2;
	auto xz2 = q.x * q.z * 2;
	auto xw2 = q.x * q.w * 2;
	auto yz2 = q.y * q.z * 2;
	auto yw2 = q.y * q.w * 2;
	auto zw2 = q.z * q.w * 2;
	auto ww2 = q.w * q.w * 2;

	// 行列の各要素を計算
	auto r00 = ww2 + 2 * q.x * q.x - 1;
	auto r01 = xy2 + zw2;
	auto r02 = xz2 - yw2;

	auto r10 = xy2 - zw2;
	auto r11 = ww2 + 2 * q.y * q.y - 1;
	auto r12 = yz2 + xw2;

	auto r20 = xz2 + yw2;
	auto r21 = yz2 - xw2;
	auto r22 = ww2 + 2 * q.z * q.z - 1;

	// 計算した要素を行列に設定
	rM.mat[0][0] = r00;
	rM.mat[0][1] = r01;
	rM.mat[0][2] = r02;
	rM.mat[1][0] = r10;
	rM.mat[1][1] = r11;
	rM.mat[1][2] = r12;
	rM.mat[2][0] = r20;
	rM.mat[2][1] = r21;
	rM.mat[2][2] = r22;
	return rM;
}

Quaternion Quaternion::AngleAxis(const float& rad, const Vector3& axis)
{
	Quaternion rQ = Quaternion::IdentityQ();
	if (axis.Magnitude() <= 0.0f)
	{
		return rQ;
	}

	//正規化
	Vector3 nAxis = axis.Normalize();
	rQ.w = cos(rad / 2.0f);//実部
	rQ.x = nAxis.x * sin(rad / 2.0f);//虚部
	rQ.y = nAxis.y * sin(rad / 2.0f);//虚部
	rQ.z = nAxis.z * sin(rad / 2.0f);//虚部
	return rQ;
}

//二つの向き・回転の間を一定角速度で滑らかに保管する
//回転や方向を直線ではなく球面上の最短弧(大円)で補完する
Quaternion Quaternion::Slerp(const Quaternion& sRota, const Quaternion& eRota, float t)
{
	//範囲外の時の処理
	if (t > 1.0f)t = 1.0f;
	if (t < 0.0f)t = 0.0f;

	//単位クォータニオン
	Quaternion rQ = IdentityQ();
	//正規化できない場合
	if (sRota.Magnitude() <= 0.0f || 
		eRota.Magnitude() <= 0.0f)
	{
		//回転しないクォータニオンを返す
		return rQ;
	}
	//開始と終了のクォータニオンを正規化
	Quaternion sRotaNom = sRota.NormQ();
	Quaternion eRotaNom = eRota.NormQ();
	//開始と終了のクォータニオンが同じなら
	if (sRotaNom == eRotaNom)
	{
		//回転しないクォータニオンを返す
		return rQ;
	}

	//内積から角度を求める
	float dot = sRotaNom.DotQ(eRotaNom);
	//負の時符号反転
	if (dot < 0.0f)
	{
		dot *= -1.0f;
	}
	float rad = acos(dot);
	
	//球面線形補間の公式より
	float s1 = sin((1.0f - t) * rad) / sin(rad);
	float s2 = sin(t * rad) / sin(rad);
	rQ = (sRotaNom * s1) + (eRotaNom * s2);
	return rQ;
}

Quaternion Quaternion::CalcHorizontalQuat(const Vector3& dir)
{
	//水平方向の回転
	float angleY = atan2f(dir.x, dir.z);
	return Quaternion::AngleAxis(angleY, Vector3::Up());
}

Quaternion Quaternion::CalcVerticalQuat(const Vector3& dir)
{
	//水平方向の長さ
	float horizontalLength = sqrtf(dir.x * dir.x + dir.z * dir.z);
	//角度
	float angleX = atan2f(dir.y, horizontalLength);
	return Quaternion::AngleAxis(-angleX, Vector3::Right());
}


//Quaternion Quaternion::LookAt(const Vector3& targetDir, const Vector3& up)
//{
//	//単位クォータニオン
//	Quaternion rQ = IdentityQ();
//	assert(targetDir.Magnitude() > 0.0f && "方向がありません");
//	Vector3 forwardDir = targetDir.Normalize();
//	//回転しないなら
//	if (forwardDir == Vector3::Forward())
//	{
//		//回転しないクォータニオンを返す
//		return rQ;
//	}
//
//	//内積から角度を求める
//	float dot = Vector3::Forward().Dot(targetDir);
//	//負の時符号反転
//	if (dot < 0.0f)
//	{
//		dot *= -1.0f;
//	}
//	float theta = acos(dot);
//
//	//回転の軸を求める
//	Vector3 axis = { 0.0f,1.0f,0.0f };
//	//真後ろを向くクォータニオン以外なら
//	if ((forwardDir * -1.0f) != Vector3::Forward())
//	{
//		//軸を外積で求める
//		axis = Vector3::Forward().Cross(targetDir);
//	}
//	//クォータニオン作成
//	rQ = AngleAxis(theta, axis);
//	return rQ;
//}
