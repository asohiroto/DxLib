#include "PlayerStateDeath.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"
#include "../../../UI/UIManager.h"

namespace
{
	//アニメーション
	const std::wstring kDeath = L"Death";
}

PlayerStateDeath::PlayerStateDeath(std::weak_ptr<Actor> player, bool isWait) :
	PlayerStateBase(player,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kDeath).c_str(), false);
	owner->SetCollState(CollisionState::Dead);
	auto status = owner->GetCharaStatus();
	//無敵
	status->SetIsNoDamage(true);
	//UI削除
	UIManager::GetInstance().AllDeleteUI();
	//強制待機状態にする(ポッドの射撃を不可能に)
	Wait();
}

PlayerStateDeath::~PlayerStateDeath()
{
}
void PlayerStateDeath::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}
void PlayerStateDeath::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	auto status = owner->GetCharaStatus();
#if _DEBUG
	//復活したら(デバッグ用)
	if (!status->IsDead())
	{
		//待機
		ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner, m_isWait));
		return;
	}
#endif
	//アニメーション終了
	if (owner->GetModel()->IsFinishAnim())
	{
		owner->Delete();
	}

	//移動量リセット
	owner->GetRb()->SetMoveVec(Vector3::Zero());
}
