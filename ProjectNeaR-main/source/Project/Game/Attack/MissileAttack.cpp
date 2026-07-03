#include "MissileAttack.h"
#include "../Actor/Character/CharacterBase.h"
#include "../../General/Collision/SphereCollider.h"
#include "../../General/Collision/Rigidbody.h"
#include "../../General/Effect/NormalEffect.h"
#include "../../General/Effect/EffekseerManager.h"

MissileAttack::MissileAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner) :
	BulletAttack(attackData, pOwner),
	m_pTarget(),
	m_trackingRate(0.0f),
	m_trackingFrame(0.0f),
	m_trackingVec(Vector3::Zero())
{

}

MissileAttack::~MissileAttack()
{
}


void MissileAttack::Draw() const
{
#if _DEBUG
	if (!m_isActive)return;
	auto color = 0x338800;
	auto coll = std::dynamic_pointer_cast<SphereCollider>(m_collisionData);
	DrawSphere3D(m_rb->m_pos.ToDxLibVector(),
		coll->GetRadius(), 16, color, color, false);
#endif

}

void MissileAttack::SetTargetTracking(std::weak_ptr<CharacterBase> pTarget, float trackingRate, float trackingFrame)
{
	//ターゲット
	m_pTarget = pTarget;
	//追跡力
	m_trackingRate = trackingRate;
	//追跡フレーム
	m_trackingFrame = trackingFrame;
}

void MissileAttack::UpdateMove()
{
	//ターゲットがいる場合
	if (!m_pTarget.expired() && m_trackingFrame > 0)
	{
		--m_trackingFrame;

		//現在のベクトル
		auto nowVec = m_rb->m_vec;
		if (nowVec.SqMagnitude() > 0.0f)
		{
			nowVec = nowVec.Normalize();
		}
		else
		{
			if (m_moveVec.SqMagnitude() > 0.0f)
			{
				nowVec = m_moveVec.Normalize();
			}
		}
		//ターゲットへの向き
		auto targetPos = m_pTarget.lock()->GetCenterPos();
		auto targetDir = targetPos - m_rb->m_pos;
		if (targetDir.SqMagnitude() > 0.0f)
		{
			targetDir = targetDir.Normalize();
		}
		//ターゲットへ少し傾ける
		nowVec = Vector3::Lerp(nowVec, targetDir, m_trackingRate);
		if (nowVec.SqMagnitude() > 0.0f)
		{
			nowVec = nowVec.Normalize();
		}
		m_rb->m_vec = nowVec * m_moveVec.Magnitude();
		m_trackingVec = m_rb->m_vec;
	}
	else
	{
		m_rb->m_vec = m_trackingVec;
	}
	//移動処理
	m_rb->SetPos(m_rb->GetNextPos());
	m_rb->SetVec(Vector3::Zero());
}
