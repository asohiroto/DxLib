#include "EnemyBulletAttack.h"
#include "../../General/Collision/SphereCollider.h"
#include "../../General/Collision/Rigidbody.h"
#include "../../General/Effect/NormalEffect.h"
#include "../../General/Effect/EffekseerManager.h"
#include "../Actor/Character/CharacterBase.h"

EnemyBulletAttack::EnemyBulletAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner):
	BulletAttack(attackData,pOwner),
	m_isDestructible(true)
{
	
}

EnemyBulletAttack::~EnemyBulletAttack()
{
}

void EnemyBulletAttack::Draw() const
{
#if _DEBUG
	if (!m_isActive)return;
	auto color = 0x888888;
	//‰ó‚¹‚é‚È‚ç
	if (m_isDestructible)
	{
		color = 0xff0000;
	}
	auto coll = std::dynamic_pointer_cast<SphereCollider>(m_collisionData);
	DrawSphere3D(m_rb->m_pos.ToDxLibVector(),
		coll->GetRadius(), 16, color, color, false);
#endif
	
}

void EnemyBulletAttack::OnCollide(const std::shared_ptr<Collidable> other)
{
	//UŒ‚‚É“–‚½‚Á‚½‰ó‚ê‚é‚©
	if (other->GetGameTag() == GameTag::Attack)
	{
		if (m_pOwner.expired())return;
		if(std::dynamic_pointer_cast<AttackBase>(other)->GetOwnerTag() == m_pOwner.lock()->GetGameTag())return;

		//‰ó‚ê‚é‚È‚ç
		if (m_isDestructible)
		{
			//“–‚½‚Á‚½ƒtƒ‰ƒO‚ğtrue‚É
			m_isHit = true;
			return;
		}
	}
	//’Êí‚Ìˆ—
	BulletAttack::OnCollide(other);
}
