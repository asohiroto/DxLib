#include "Rigidbody.h"
#include "../Math/Vector2.h"

namespace
{
	//0に近い値
	constexpr float kEpsilon = 0.0001f;
}

Rigidbody::Rigidbody():
	m_pos(),
	m_vec(),
	m_isGravity(true) ,//重力を受ける
	m_isMyTimeScale(false),
	m_myTimeScale(1.0f)
{
}


Vector3 Rigidbody::GetVecWithTS()const
{
	return (m_vec * m_myTimeScale);
}

Vector3 Rigidbody::GetMoveVec()
{
	Vector3 moveVec = m_vec;
	moveVec.y = 0.0f; //Y軸は無視
	//X,Zのベクトルを返す
	if (moveVec.Magnitude() <= 0.0f)
	{
		//移動ベクトルが0なら0を返す
		moveVec = Vector3{ 0.0f,0.0f,0.0f };
	}
	
	return moveVec;
}

void Rigidbody::SetMoveVec(Vector3 vec)
{
	m_vec.x = vec.x;
	m_vec.z = vec.z;
}

Vector3 Rigidbody::GetNextPos() const
{
	return m_pos + GetVecWithTS();
}

void Rigidbody::SpeedDown(float decRate)
{
	//減速
	m_vec.x *= decRate;
	m_vec.z *= decRate;
	//ほぼ0なら
	if (m_vec.XZ().Magnitude() <= kEpsilon)
	{
		m_vec.x = 0.0f;
		m_vec.z = 0.0f;
	}
}
