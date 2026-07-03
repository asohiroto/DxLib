#include "DebugEnemy.h"
#include "../../../General/Collision/Rigidbody.h"
#include "../../../General/Collision/SphereCollider.h"
#include "../../../General/Input.h"
#include "../../../General/CharaStatus.h"
#include "../../../Main/Application.h"

namespace
{
	constexpr float kSpeed = 500.0f;
	constexpr float kJumpPower = 400.0f;
}

DebugEnemy::DebugEnemy(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager):
	CharacterBase(actorData, charaStatusData,Shape::Sphere, pActorManager)
{
}

DebugEnemy::~DebugEnemy()
{
}

void DebugEnemy::Init()
{
	//Physicsに登録
	Collidable::Init();
}

void DebugEnemy::Update()
{
	//入力取得
	auto& input = Input::GetInstance();
	//移動
	Vector3 vec = Vector3::Zero();
	//タイムスケールを変更
	if (input.IsTrigger("Y"))
	{
		m_rb->m_myTimeScale = 0.5f;
		m_rb->SetIsMyTimeScale(!m_rb->IsMyTimeScale());
	}

	m_charaStatus->InitHitState();
}

void DebugEnemy::Draw() const
{
	//ダメージを喰らったら色を変える
	auto color = GetColor(255, 0, 0);
	if (m_charaStatus->IsHit())
	{
		color = GetColor(0, 255, 255);
	}

	DrawSphere3D(VGet(m_rb->m_pos.x, m_rb->m_pos.y, m_rb->m_pos.z), m_actorData->GetCollRadius(), 16, color, 0, true);
	//体力
	DrawFormatString(0, 120, 0xffffff, L"Enemyの体力は = %d", m_charaStatus->GetNowHP());
}

void DebugEnemy::End()
{
	//Physicsから解除
	Collidable::End();
}

void DebugEnemy::OnCollide(const std::shared_ptr<Collidable> other)
{
}

void DebugEnemy::Complete()
{
	//Rigidbodyの位置を更新
	m_rb->SetPos(m_rb->GetNextPos());
}
