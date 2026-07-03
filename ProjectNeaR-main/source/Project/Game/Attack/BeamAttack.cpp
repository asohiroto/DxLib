#include "BeamAttack.h"
#include "../../General/Collision/CapsuleCollider.h"
#include "../../General/Collision/Rigidbody.h"
#include "../../General/Effect/NormalEffect.h"
#include "../../Main/Application.h"

BeamAttack::BeamAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner) :
	CapsuleAttackBase(attackData, pOwner),
	m_rotaAngle(0.0f),
	m_axis(Vector3::Zero())
{
}

BeamAttack::~BeamAttack()
{
}

void BeamAttack::Update()
{
	//基底の更新
	AttackBase::Update();
	//カプセルの更新
	CapsuleAttackBase::Update();
	//回転
	if (m_rotaAngle != 0.0f && m_axis.SqMagnitude() > 0.0f)
	{
		Vector3 startPos = m_rb->GetPos();
		Vector3 endPos = std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetEndPos();
		//向き
		Vector3 sToE = endPos - startPos;
		//長さ
		float len = sToE.Magnitude();
		if (sToE.SqMagnitude() > 0.0f)
		{
			sToE = sToE.Normalize();
		}
		
		sToE = Quaternion::AngleAxis(m_rotaAngle * MyMath::DEG_2_RAD * Application::GetInstance().GetTimeScale(), m_axis) * sToE;
		Vector3 nextEndPos = startPos + (sToE * len);
		std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->SetEndPos(nextEndPos);

		if (!m_attackEffect.expired())
		{
			m_attackEffect.lock()->LookAt(sToE);
		}
	}
}

void BeamAttack::Draw() const
{
#if _DEBUG
	//衝突判定
	DrawCapsule3D(
		m_rb->m_pos.ToDxLibVector(),
		std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetEndPos().ToDxLibVector(),
		std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetRadius(),
		8,
		0xff00ff,
		0xff00ff,
		false//地面にいると塗りつぶされる
	);
#endif
}
