#include "Boss3StateHit.h"
#include "Boss3StateIdle.h"
#include "../EnemyBase.h"
#include "../EnemyStateDeath.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/Input.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/CharaStatus.h"

namespace
{
	//アニメーション
	const std::wstring kHit = L"Hit";
}

Boss3StateHit::Boss3StateHit(std::weak_ptr<Actor> enemy, bool isWait) :
	EnemyStateBase(enemy,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kHit).c_str(), false);
	owner->SetCollState(CollisionState::Normal);
}


Boss3StateHit::~Boss3StateHit()
{

}

void Boss3StateHit::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void Boss3StateHit::Update()
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
	//ヒット中に攻撃を再度喰らったら最初から
	if (owner->GetCharaStatus()->IsHitReaction())
	{
		//最初から
		owner->GetModel()->ReplayAnim();
		return;
	}
	//モデルのアニメーションが終わったら
	if (owner->GetModel()->IsFinishAnim())
	{
		//待機
		ChangeState(std::make_shared<Boss3StateIdle>(m_pOwner, m_isWait));
		return;
	}
	auto rb = owner->GetRb();
	//減速
	rb->SpeedDown();
	//浮かないように
	rb->SetVecY(0.0f);
}
