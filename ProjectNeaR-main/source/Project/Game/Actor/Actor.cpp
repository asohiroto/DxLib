#include "Actor.h"
#include "../../General/Collision/Rigidbody.h"
#include "../../General/Collision/Physics.h"
#include "../../General/Collision/SphereCollider.h"
#include "../../General/Collision/CapsuleCollider.h"
#include "../../General/Collision/PolygonCollider.h"
#include "../../General/Collision/TorusCollider.h"
#include "../../General/Model.h"
#include "../../General/AssetManager.h"

Actor::Actor(std::shared_ptr<ActorData> actorData,Shape shape, std::weak_ptr<ActorManager> pActorManager):
	Collidable(shape),
	m_pActorManager(pActorManager),
	m_actorData(actorData),
	m_isDelete(false),
	m_id(0),
	m_isSetId(false)
{
	if (!m_actorData)return;
	//モデル取得
	auto& assetManager = AssetManager::GetInstance();
	int handle = assetManager.GetModelHandle(m_actorData->GetModelPath());
	m_model = std::make_shared<Model>(handle, m_actorData->GetPos().ToDxLibVector());
	//大きさ
	m_model->SetScale(m_actorData->GetScale().ToDxLibVector());
	//回転量
	m_model->SetRot(m_actorData->GetRot().ToDxLibVector());

	//座標
	m_rb->SetPos(m_actorData->GetPos());
	//コライダーの設定
	switch (shape)
	{
	case Shape::Sphere:
	{
		auto coll = std::dynamic_pointer_cast<SphereCollider>(m_collisionData);
		coll->SetRadius(m_actorData->GetCollRadius());
		break;
	}
	case Shape::Capsule:
	{
		auto coll = std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData);
		coll->SetRadius(m_actorData->GetCollRadius());
		Vector3 endPos = m_rb->m_pos;
		endPos.y += m_actorData->GetCollHeight();
		coll->SetEndPos(endPos);
		break;
	}
	case Shape::Polygon:
	{
		auto coll = std::dynamic_pointer_cast<PolygonCollider>(m_collisionData);
		coll->SetModelHandle(handle);
		break;
	}
	case Shape::Torus:
	{
		auto coll = std::dynamic_pointer_cast<TorusCollider>(m_collisionData);
		coll->SetRadius(m_actorData->GetCollRadius());
		coll->SetRange(m_actorData->GetCollHeight());
		break;
	}
	default:
		break;
	};
	AllSetting(m_actorData->GetPriority(), m_actorData->GetGameTag(), m_actorData->IsTrough(), m_actorData->IsTrigger(), m_actorData->IsGravity());

	//IDの設定
	SetID(m_actorData->GetActorID());
}

void Actor::SetID(int id)
{
	//すでに登録してるなら
	if (m_isSetId)return;
	m_id = id;
	m_isSetId = true;
}

Vector3 Actor::GetPos() const
{
	return m_rb->m_pos;
}

Vector3 Actor::GetNextPos() const
{
	return m_rb->GetNextPos();
}

void Actor::SetTimeScale(float scale)
{
	Collidable::SetTimeScale(scale);
	//モデルにも
	if (m_model)
	{
		m_model->SetTimeScale(scale);
	}
}

float Actor::GetTimeScale()const
{
	return m_rb->GetMyTimeScale();
}

void Actor::EnableIsMyScale()
{
	m_rb->SetIsMyTimeScale(true);
}

void Actor::DisableIsMyScale()
{
	m_rb->SetIsMyTimeScale(false);
}
