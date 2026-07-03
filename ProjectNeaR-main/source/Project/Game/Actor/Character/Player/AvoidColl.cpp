#include "AvoidColl.h"
#include "../../../Attack/AttackBase.h"
#include "../../../../General/Collision/SphereCollider.h"
#include "../../../../General/Collision/Rigidbody.h"

AvoidColl::AvoidColl(float radius, unsigned int playerID):
	Collidable(Shape::Sphere),
	m_isHit(false),
	m_playerID(playerID)
{
	//半径
	std::dynamic_pointer_cast<SphereCollider>(m_collisionData)->SetRadius(radius);
	//設定
	AllSetting(Priority::Low, GameTag::Avoid, true, true, false);
}

AvoidColl::~AvoidColl()
{
}

void AvoidColl::Update(Vector3 nextPos, bool isEnableColl)
{
	//位置更新
	m_rb->SetPos(nextPos);
	//ヒット判定リセット
	m_isHit = false;
	//当たり判定をするか
	if (isEnableColl)
	{
		EnableColl();
	}
	else
	{
		DisableColl();
	}
}
void AvoidColl::Draw()const
{
	DrawSphere3D(m_rb->GetPos().ToDxLibVector(),
		std::dynamic_pointer_cast<SphereCollider>(m_collisionData)->GetRadius(),
		16,
		0x00ff00,
		0x00ff00,
		false);
}

void AvoidColl::OnCollide(const std::shared_ptr<Collidable> other)
{
	//攻撃に当たった場合
	if (other->GetGameTag() == GameTag::Attack)
	{
		auto attack = std::dynamic_pointer_cast<AttackBase>(other);
		//敵の攻撃かをチェック
		if (attack->GetOwnerTag() == GameTag::Enemy)
		{
			m_isHit = !attack->IsHitID(m_playerID);
		}
	}
}
