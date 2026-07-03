#include "CapsuleAttackBase.h"
#include "../../General/Collision/Rigidbody.h"
#include "../../General/Collision/CapsuleCollider.h"
#include "../../Game/Actor/Character/CharacterBase.h"
#include "../../General/CSV/AttackData.h"

CapsuleAttackBase::CapsuleAttackBase(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner):
	AttackBase(Shape::Capsule, attackData, pOwner)
{
}

void CapsuleAttackBase::Update()
{
	//Šî€À•W‚ÌXV
	if (m_originPosType == AttackData::AttackOriginPosType::Owner)
	{
		if (m_pOwner.expired()) return;
		auto ownerColl = m_pOwner.lock();
		m_oriPos = ownerColl->GetNextPos();
	}
	else
	{
		auto cap = std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData);
		m_oriPos = (m_rb->m_pos + cap->GetEndPos()) * 0.5f;
	}
}

void CapsuleAttackBase::SetStartPos(const Vector3& pos)
{
	m_rb->m_pos = pos;
}

void CapsuleAttackBase::SetEndPos(const Vector3& pos)
{
	auto cap = std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData);
	cap->SetEndPos(pos);
}

void CapsuleAttackBase::SetRadius(float radius)
{
	auto cap = std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData);
	cap->SetRadius(radius);
}

