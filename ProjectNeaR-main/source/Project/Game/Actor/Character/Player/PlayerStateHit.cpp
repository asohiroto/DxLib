#include "PlayerStateHit.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include "PlayerStateFall.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"

namespace
{
	//アニメーション
	const std::wstring kHit1 = L"Hit1";
	const std::wstring kHit2 = L"Hit2";
}

PlayerStateHit::PlayerStateHit(std::weak_ptr<Actor> player,bool isWait) :
	PlayerStateBase(player,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	if (MyMath::IsRand())
	{
		owner->GetModel()->SetAnim(owner->GetAnim(kHit1).c_str(), false);
	}
	else
	{
		owner->GetModel()->SetAnim(owner->GetAnim(kHit2).c_str(), false);
	}
	owner->SetCollState(CollisionState::Normal);

	auto status = owner->GetCharaStatus();

	//無敵に
	status->SetIsNoDamage(true);
}

PlayerStateHit::~PlayerStateHit()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	auto status = owner->GetCharaStatus();

	//無敵解除
	status->SetIsNoDamage(false);
}
void PlayerStateHit::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}
void PlayerStateHit::Update()
{
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	//モデルのアニメーションが終わったら
	if (owner->GetModel()->IsFinishAnim())
	{
		//待機
		ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner,m_isWait));
		return;
	}
	//減速
	owner->GetRb()->SpeedDown();
}
