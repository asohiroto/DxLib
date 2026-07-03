#include "PlayerStateIdle.h"
#include "Player.h"
#include "PlayerStateMoving.h"
#include "PlayerStateAvoid.h"
#include "PlayerStateFall.h"
#include "PlayerStateJump.h"
#include "PlayerStateHit.h"
#include "PlayerStateDeath.h"
#include "PlayerStateLightAttack.h"
#include "PlayerStateHeavyAttack.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"

namespace
{
	//アニメーション
	const std::wstring kIdle = L"Idle";
}

PlayerStateIdle::PlayerStateIdle(std::weak_ptr<Actor> player, bool isWait):
	PlayerStateBase(player, isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	owner->SetCollState(CollisionState::Normal);

	//地面に付いてるなら
	if (owner->IsFloor())
	{
		//回避可能
		owner->SetIsAvoidable(true);
	}

	//描画する
	owner->SetIsDraw(true);
}

PlayerStateIdle::~PlayerStateIdle()
{
}
void PlayerStateIdle::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}
void PlayerStateIdle::Update()
{
	auto& input = Input::GetInstance();
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	auto status = owner->GetCharaStatus();

	//強制待機状態ではないなら
	if (!m_isWait)
	{
		//回避
		if (input.IsBuffered("B") && owner->IsAvoidable())
		{
			ChangeState(std::make_shared<PlayerStateAvoid>(m_pOwner,m_isWait));
			return;
		}
		//死亡
		if (status->IsDead())
		{
			ChangeState(std::make_shared<PlayerStateDeath>(m_pOwner, m_isWait));
			return;
		}
		//落下
		if (owner->IsFall())
		{
			ChangeState(std::make_shared<PlayerStateFall>(m_pOwner, m_isWait));
			return;
		}
		//やられ
		if (owner->GetCharaStatus()->IsHitReaction())
		{
			ChangeState(std::make_shared<PlayerStateHit>(m_pOwner, m_isWait));
			return;
		}
		//攻撃
		if (input.IsBuffered("X"))
		{
			ChangeState(std::make_shared<PlayerStateLightAttack>(m_pOwner, m_isWait, false, status->IsNoDamage()));
			return;
		}
		if (input.IsBuffered("Y"))
		{
			ChangeState(std::make_shared<PlayerStateHeavyAttack>(m_pOwner, m_isWait, false, status->IsNoDamage()));
			return;
		}
		//ジャンプ
		if (owner->IsJumpable() && input.IsBuffered("A"))
		{
			ChangeState(std::make_shared<PlayerStateJump>(m_pOwner, m_isWait));
			return;
		}
		//入力があるなら移動
		if (input.GetStickInfo().IsLeftStickInput())
		{
			//歩き
			ChangeState(std::make_shared<PlayerStateMoving>(m_pOwner, m_isWait, false));
			return;
		}
	}

	//アニメーション切り替え
	ChangeAnim(owner);

	//移動量リセット
	owner->GetRb()->SetMoveVec(Vector3::Zero());
}

void PlayerStateIdle::ChangeAnim(std::shared_ptr<Player> owner)
{
	owner->GetModel()->SetAnim(owner->GetAnim(kIdle).c_str(), true);
}
