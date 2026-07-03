#include "EnemyStateFall.h"
#include "EnemyStateIdle.h"
#include "EnemyStateHit.h"
#include "EnemyStateDown.h"
#include "EnemyStateDeath.h"
#include "EnemyBase.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"

namespace
{
	//アニメーション
	const std::wstring kNormalFall = L"Fall";
	const std::wstring kHitFall = L"FallHitAir";
	//落下中の減速率
	constexpr float kHorizontalSpeedDownRate = 0.8f;
}

EnemyStateFall::EnemyStateFall(std::weak_ptr<Actor> enemy, bool isWait, bool isHit):
	EnemyStateBase(enemy,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	if (!isHit)
	{
		//通常落下
		owner->GetModel()->SetAnim(owner->GetAnim(kNormalFall).c_str(), true);
	}
	else
	{
		//攻撃を喰らって落下
		owner->GetModel()->SetAnim(owner->GetAnim(kHitFall).c_str(), true);
	}
	owner->SetCollState(CollisionState::Fall);
	m_isHit = isHit;
	//横の移動量も現象
	owner->GetRb()->SpeedDown(kHorizontalSpeedDownRate);
}

EnemyStateFall::~EnemyStateFall()
{
}
void EnemyStateFall::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void EnemyStateFall::Update()
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
	//ヒット状態
	if (owner->GetCharaStatus()->IsHitReaction())
	{
		//ヒット状態ならヒットステートへ
		ChangeState(std::make_shared<EnemyStateHit>(m_pOwner, m_isWait));
		return;
	}
	//地面に付いたら
	if (owner->IsFloor())
	{
		if(m_isHit)
		{
			//ダウン状態へ
			ChangeState(std::make_shared<EnemyStateDown>(m_pOwner, m_isWait));
			return;
		}
		else
		{
			//待機状態へ
			ChangeState(std::make_shared<EnemyStateIdle>(m_pOwner, m_isWait));
			return;
		}
	}
}
