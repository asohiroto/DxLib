#include "DebugAttack.h"
#include "../../../General/Collision/Rigidbody.h"
#include "../../../General/Collision/SphereCollider.h"
#include "../../../General/Input.h"
#include "../../../Main/Application.h"
#include "../Character/Player/Player.h"

namespace
{
	constexpr float kSpeed = 500.0f;
	constexpr float kJumpPower = 400.0f;
}

DebugAttack::DebugAttack(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager) :
	Actor(actorData, Shape::Sphere, pActorManager)
{
}

DebugAttack::~DebugAttack()
{
}

void DebugAttack::Init()
{
	//PhysicsÇ…ìoò^
	Collidable::Init();
}

void DebugAttack::Update()
{
	//à⁄ìÆ
	Vector3 vec = Vector3::Zero();
	m_rb->SetVec(vec);
}

void DebugAttack::Draw() const
{
	DrawSphere3D(VGet(m_rb->m_pos.x, m_rb->m_pos.y, m_rb->m_pos.z), m_actorData->GetCollRadius(), 16, 0xff0000, 0, false);
	DrawFormatString(0, 120, 0xffffff, L"EnemyÇÃTimeScale = %.1f", m_rb->m_myTimeScale);
}

void DebugAttack::End()
{
	//PhysicsÇ©ÇÁâèú
	Collidable::End();
}

void DebugAttack::OnCollide(const std::shared_ptr<Collidable> other)
{
	if (other->GetGameTag() == GameTag::Player)
	{
		std::dynamic_pointer_cast<Player>(other)->GetCharaStatus()->OnDamage(10, 10, CharaStatus::AttackWeight::Heaviest);
	}
}

void DebugAttack::Complete()
{
	//RigidbodyÇÃà íuÇçXêV
	m_rb->SetPos(m_rb->GetNextPos());
}
