#include "CheckPoint.h"
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
	const std::wstring kCheckPointBeforePath = L"CheckPointBefore";
	const std::wstring kCheckPointAfterPath = L"CheckPointAfter";
}

CheckPoint::CheckPoint(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager) :
	Actor(actorData, Shape::Sphere, pActorManager),
	m_isPass(false)
{
}

CheckPoint::~CheckPoint()
{
	if (m_effect.expired())return;
	m_effect.lock()->Delete();
}

void CheckPoint::Init()
{
	//エフェクト
	if (m_effect.expired())
	{
		m_effect = EffekseerManager::GetInstance().CreateEffect(kCheckPointBeforePath, m_actorData->GetPos());
	}
	if (m_isThrough)return;
	Collidable::Init();
}

void CheckPoint::OnCollide(const std::shared_ptr<Collidable> other)
{
	if ((other->GetGameTag() == GameTag::Player) && !m_isPass)
	{
		auto player = std::dynamic_pointer_cast<Player>(other);
		player->SetRespawnPos(m_rb->GetPos());		//リスポーン位置
		player->GetCharaStatus()->FullRecovery();	//回復
		m_isPass = true;
	}
}

void CheckPoint::Draw() const
{
#if _DEBUG
	DrawSphere3D(m_rb->GetPos().ToDxLibVector(), m_actorData->GetCollRadius(), 32, 0x00ffff, 0x00ffff, false);
#endif
}

void CheckPoint::Complete()
{
	//通過したのなら
	if (m_isPass && !m_effect.expired())
	{
		//エフェクトを削除して通過したことがわかるエフェクトを出す
		m_effect.lock()->Delete();
		EffekseerManager::GetInstance().CreateEffect(kCheckPointAfterPath, m_actorData->GetPos());
	}
}

void CheckPoint::End()
{
	Collidable::End();
}
