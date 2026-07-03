#include "SphereAttackBase.h"
#include "../../General/Collision/Rigidbody.h"
#include "../../General/Collision/SphereCollider.h"
#include "../../General/CSV/AttackData.h"
#include "../../Game/Actor/Character/CharacterBase.h"

SphereAttackBase::SphereAttackBase(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner) :
	AttackBase(Shape::Sphere, attackData, pOwner)
{
}

void SphereAttackBase::Update()
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
		m_oriPos = m_rb->m_pos;
	}
}

void SphereAttackBase::SetPos(const Vector3& pos)
{
	m_rb->m_pos = pos;
}

void SphereAttackBase::SetRadius(float radius)
{
	std::dynamic_pointer_cast<SphereCollider>(m_collisionData)->SetRadius(radius);
}
