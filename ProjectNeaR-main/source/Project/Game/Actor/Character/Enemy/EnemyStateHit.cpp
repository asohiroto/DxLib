#include "EnemyStateHit.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"
#include "EnemyStateFall.h"
#include "EnemyStateDeath.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"

namespace
{
	//アニメーション
	const std::wstring kNormalHit = L"Hit";
	const std::wstring kAirHit = L"HitAir";

	//最低上昇量
	constexpr float kMinUpVecY = 4.0f;
	//横方向減少
	constexpr float kHorizontalSpeedDownRate = 0.7f;
}

EnemyStateHit::EnemyStateHit(std::weak_ptr<Actor> enemy, bool isWait) :
	EnemyStateBase(enemy,isWait),
	m_isFall(false)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	//最低上昇量保証
	MinUpVecY(owner);
	//地上か空中かでアニメーション変更
	ChangeNormalOrFall(owner);
	//横の移動量も現象
	owner->GetRb()->SpeedDown(kHorizontalSpeedDownRate);
}


EnemyStateHit::~EnemyStateHit()
{
}
void EnemyStateHit::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}
void EnemyStateHit::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	
	//ステータス
	auto status = owner->GetCharaStatus();
	//死亡
	if (status->IsDead())
	{
		ChangeState(std::make_shared<EnemyStateDeath>(m_pOwner, m_isWait));
		return;
	}
	//状態に合わせてアニメーション変更
	ChangeNormalOrFall(owner);
	//ヒット中に攻撃を再度喰らったら最初から
	if (owner->GetCharaStatus()->IsHitReaction())
	{
		//最低上昇量保証
		MinUpVecY(owner);
		//横の移動量も現象
		owner->GetRb()->SpeedDown(kHorizontalSpeedDownRate);

		//最初から
		owner->GetModel()->ReplayAnim();
		return;
	}
	//モデルのアニメーションが終わったら
	if (owner->GetModel()->IsFinishAnim())
	{
		if(!m_isFall)
		{
			//待機
			ChangeState(std::make_shared<EnemyStateIdle>(m_pOwner, m_isWait));
			return;
		}
		else
		{
			//落下状態へ
			ChangeState(std::make_shared<EnemyStateFall>(m_pOwner, m_isWait, true));
			return;
		}
	}
	//減速
	owner->GetRb()->SpeedDown();
}

void EnemyStateHit::MinUpVecY(std::shared_ptr<EnemyBase> owner)
{
	//空中なら
	if (!owner->IsFloor())
	{
		//少し浮かせる
		if (owner->GetRb()->GetVec().y < kMinUpVecY)
		{
			owner->GetRb()->SetVecY(kMinUpVecY);
		}
	}
}

void EnemyStateHit::ChangeNormalOrFall(std::shared_ptr<EnemyBase> owner)
{
	if (owner->IsFloor())
	{
		owner->GetModel()->SetAnim(owner->GetAnim(kNormalHit).c_str(), false);
		owner->SetCollState(CollisionState::Normal);
		m_isFall = false;
	}
	else
	{
		owner->GetModel()->SetAnim(owner->GetAnim(kAirHit).c_str(), false);
		owner->SetCollState(CollisionState::Fall);
		m_isFall = true;
	}
}