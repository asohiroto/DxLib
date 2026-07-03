#include "Boss4StateHit.h"
#include "Boss4StateIdle.h"
#include "Boss4StateDeath.h"
#include "../EnemyBase.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/Input.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/CharaStatus.h"

namespace
{
	//アニメーション
	const std::wstring kHit = L"Hit";
}

Boss4StateHit::Boss4StateHit(std::weak_ptr<Actor> enemy, bool isWait) :
	EnemyStateBase(enemy,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kHit).c_str(), false);
	owner->SetCollState(CollisionState::Normal);
}


Boss4StateHit::~Boss4StateHit()
{

}

void Boss4StateHit::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void Boss4StateHit::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());

	//ステータス
	auto status = owner->GetCharaStatus();
	//死亡
	if (status->IsDead())
	{
		ChangeState(std::make_shared<Boss4StateDeath>(m_pOwner, m_isWait));
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
		ChangeState(std::make_shared<Boss4StateIdle>(m_pOwner, m_isWait));
		return;
	}
	auto rb = owner->GetRb();
	//減速
	rb->SpeedDown();
	//浮かないように
	rb->SetVecY(0.0f);
}
