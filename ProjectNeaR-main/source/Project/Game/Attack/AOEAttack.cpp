#include "AOEAttack.h"
#include "../../General/Collision/SphereCollider.h"
#include "../../General/Collision/Rigidbody.h"

AOEAttack::AOEAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner) :
	SphereAttackBase(attackData, pOwner)
{
}

AOEAttack::~AOEAttack()
{
}

void AOEAttack::Update()
{
	//AttackBaseの更新
	AttackBase::Update();
	//球の更新
	SphereAttackBase::Update();
}

void AOEAttack::Draw() const
{
#if _DEBUG
	auto coll = std::dynamic_pointer_cast<SphereCollider>(m_collisionData);
	DrawSphere3D(m_rb->m_pos.ToDxLibVector(),
		coll->GetRadius(), 16, 0xff0000, 0xffffff, false);
#endif
}

void AOEAttack::OnCollide(const std::shared_ptr<Collidable> other)
{
	AttackBase::OnCollide(other);
}
