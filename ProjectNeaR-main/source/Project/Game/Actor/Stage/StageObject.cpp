#include "StageObject.h"
#include "../../../General/Collision/ColliderBase.h"
#include "../../../General/Collision/Rigidbody.h"
#include "../../../General/Collision/Collidable.h"
#include "../../../General/Collision/PolygonCollider.h"
#include "../../../General/Effect/EffekseerManager.h"
#include "../../../General/Effect/TrackActorEffect.h"
#include "../../../General/Model.h"



StageObject::StageObject(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager):
	Actor(actorData,Shape::Polygon,pActorManager),
	m_isDraw(true)
{
}

StageObject::~StageObject()
{
}

void StageObject::Init()
{
	if(m_actorData->GetCSVPathData() != L"None")
	{
		//エフェクトのパスがあるならエフェクトを再生
		auto& effekseerManager = EffekseerManager::GetInstance();
		effekseerManager.CreateEffect(m_actorData->GetCSVPathData(), GetPos());
	}

	m_isDraw = m_isTrigger;
	if (m_isThrough)return;
	Collidable::Init();
	MV1SetupCollInfo(m_model->GetModelHandle(), -1);
}

void StageObject::Update()
{
	m_model->Update();
	m_model->ApplyMat();
	//当たり判定をする
	if (!m_isThrough)
	{
		MV1RefreshCollInfo(m_model->GetModelHandle(), -1);
	}
}

void StageObject::OnCollide(const std::shared_ptr<Collidable> other)
{
}

void StageObject::Draw() const
{
	//描画するか
	if (!m_isDraw)return;
	m_model->Draw();
}

void StageObject::DrawShadow() const
{
	Draw();
}

void StageObject::Complete()
{
}

void StageObject::End()
{
	m_model->End();
	if (m_isThrough)return;
	Collidable::End();
}

float StageObject::GetScaleZ() const
{
	return m_model->GetScale().z;
}

Vector3 StageObject::GetDir() const
{
	return m_model->GetDir();
}
