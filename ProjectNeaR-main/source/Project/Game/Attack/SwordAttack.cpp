#include "SwordAttack.h"
#include "../../General/Collision/CapsuleCollider.h"
#include "../../General/Collision/Rigidbody.h"

SwordAttack::SwordAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner) :
	CapsuleAttackBase(attackData, pOwner)
{
}

SwordAttack::~SwordAttack()
{
}

void SwordAttack::Update()
{
	//基底の更新
	AttackBase::Update();
	//カプセルの更新
	CapsuleAttackBase::Update();
}

void SwordAttack::Draw() const
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
