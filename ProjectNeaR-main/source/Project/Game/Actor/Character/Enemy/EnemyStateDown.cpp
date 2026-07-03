#include "EnemyStateDown.h"
#include "EnemyStateIdle.h"
#include "EnemyStateHit.h"
#include "EnemyStateDeath.h"
#include "EnemyBase.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"

namespace
{
	//アニメーション
	const std::wstring kDown = L"Down";
	//フレーム数
	constexpr float kDownFrame = 5.0f * 60.0f;
}

EnemyStateDown::EnemyStateDown(std::weak_ptr<Actor> enemy, bool isWait):
	EnemyStateBase(enemy,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kDown).c_str(), true);
	owner->SetCollState(CollisionState::Normal);
}

EnemyStateDown::~EnemyStateDown()
{
}

void EnemyStateDown::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void EnemyStateDown::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());

	//強制待機状態へ
	if (m_isWait)
	{
		ChangeState(std::make_shared<EnemyStateIdle>(m_pOwner, m_isWait));
		return;
	}

	//ステータス
	auto status = owner->GetCharaStatus();
	//死亡
	if (status->IsDead())
	{
		ChangeState(std::make_shared<EnemyStateDeath>(m_pOwner, m_isWait));
		return;
	}

	//カウント
	CountFrame();

	//一定フレーム経過後
	if (m_frame >= kDownFrame)
	{
		//待機
		ChangeState(std::make_shared<EnemyStateIdle>(m_pOwner, m_isWait));
		return;
	}
	
	//移動量リセット
	auto rb = owner->GetRb();
	rb->SetMoveVec(Vector3::Zero());
	//上昇もしない
	if (rb->GetVec().y > 0.0f)
	{
		rb->SetVecY(0.0f);
	}
}
