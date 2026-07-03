#include "Goal.h"
#include "../ActorManager.h"
#include "../../../General/Collision/ColliderBase.h"
#include "../../../General/Collision/Rigidbody.h"
#include "../../../General/Collision/Collidable.h"
#include "../../../General/Collision/PolygonCollider.h"
#include "../../../General/Effect/EffekseerManager.h"
#include "../../../General/Effect/NormalEffect.h"
#include "../../../General/AssetManager.h"
#include "../Character/Player/Player.h"

namespace
{
	const std::wstring kEffectPath = L"Destination";
}

Goal::Goal(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager) :
	Actor(actorData, Shape::Sphere, pActorManager)
{
	
}

Goal::~Goal()
{
	if (m_effect.expired())return;
	m_effect.lock()->Delete();
}

void Goal::Init()
{
	//エフェクト
	if (m_effect.expired())
	{
		m_effect = EffekseerManager::GetInstance().CreateEffect(kEffectPath, m_actorData->GetPos());
	}
	if (m_isThrough)return;
	Collidable::Init();
}

void Goal::Update()
{
	
}

void Goal::OnCollide(const std::shared_ptr<Collidable> other)
{
	if (other->GetGameTag() == GameTag::Player)
	{
		std::dynamic_pointer_cast<Player>(other)->SetIsGoal(true);
	}
}

void Goal::Draw() const
{
#if _DEBUG
	DrawSphere3D(m_rb->GetPos().ToDxLibVector(), m_actorData->GetCollRadius(), 32, 0x00ffff, 0x00ffff, false);
#endif
}

void Goal::Complete()
{
}

void Goal::End()
{
	//削除
	if (!m_effect.expired())
	{
		m_effect.lock()->Delete();
	}
	if (m_isThrough)return;
	Collidable::End();
}
