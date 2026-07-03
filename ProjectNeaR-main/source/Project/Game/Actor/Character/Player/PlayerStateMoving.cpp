#include "PlayerStateMoving.h"
#include "Player.h"
#include "PlayerStateIdle.h"
#include "PlayerStateJump.h"
#include "PlayerStateAvoid.h"
#include "PlayerStateFall.h"
#include "PlayerStateHit.h"
#include "PlayerStateDeath.h"
#include "PlayerStateHeavyAttack.h"
#include "PlayerStateLightAttack.h"
#include "../../../../General/Input.h"
#include "../../../../General/Model.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"
#include "../../../../General/Sound/SoundManager.h"
#include "../../../../General/Sound/SE.h"

namespace
{
	constexpr float kRunSpeedRata = 1.5f;

	//小走り
	const std::wstring kJog = L"Jog";
	//歩き
	const std::wstring kWalk = L"Walk";
	//走る
	const std::wstring kRun = L"Run";

	//SE
	const std::wstring kWalkSE = L"Walk";
	const std::wstring kRunSE = L"Run";
}

PlayerStateMoving::PlayerStateMoving(std::weak_ptr<Actor> player, bool isWait, bool isDash) :
	PlayerStateBase(player,isWait),
	m_speed(0.0f),
	m_isDash(isDash)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	//ステータス
	auto status = owner->GetCharaStatus();
	m_speed = status->GetMS();

	//SEのパス
	std::wstring sePath = kWalkSE;

	//走り
	if(m_isDash)
	{
		//武器は持たない
		owner->PutAwaySword();
		//アニメーション
		owner->GetModel()->SetAnim(owner->GetAnim(kRun).c_str(), true);
		//速度を少し早く
		m_speed *= kRunSpeedRata;

		//走りSE
		sePath = kRunSE;
	}
	owner->SetCollState(CollisionState::Move);

	//SE再生
	m_moveSE = SoundManager::GetInstance().PlayLoopSE(owner->GetSEPath(sePath));

	//地面に付いてるなら
	if (owner->IsFloor())
	{
		//回避可能
		owner->SetIsAvoidable(true);
	}
}

PlayerStateMoving::~PlayerStateMoving()
{
	if (m_moveSE.expired())return;
	m_moveSE.lock()->Delete();
}

void PlayerStateMoving::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void PlayerStateMoving::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());

	//強制待機
	if (m_isWait)
	{
		//待機
		ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner, m_isWait));
		return;
	}


	auto status = owner->GetCharaStatus();
	auto& input = Input::GetInstance();
	//回避
	if (input.IsBuffered("B") && owner->IsAvoidable())
	{
		//回避
		ChangeState(std::make_shared<PlayerStateAvoid>(m_pOwner, m_isWait));
		return;
	}
	//死亡
	if (status->IsDead())
	{
		ChangeState(std::make_shared<PlayerStateDeath>(m_pOwner, m_isWait));
		return;
	}
	//やられ
	if (status->IsHitReaction())
	{
		ChangeState(std::make_shared<PlayerStateHit>(m_pOwner, m_isWait));
		return;
	}
	//落下
	if (owner->IsFall())
	{
		ChangeState(std::make_shared<PlayerStateFall>(m_pOwner, m_isWait));
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
		ChangeState(std::make_shared<PlayerStateHeavyAttack>(m_pOwner, m_isWait, m_isDash, status->IsNoDamage()));
		return;
	}
	//待機
	if (!input.GetStickInfo().IsLeftStickInput())
	{
		//待機
		ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner, m_isWait));
		return;
	}
	//ジャンプ
	if (owner->IsJumpable() && input.IsBuffered("A"))
	{
		ChangeState(std::make_shared<PlayerStateJump>(m_pOwner, m_isWait));
		return;
	}

	//アニメーション
	ChangeWalkAnim(owner, input);

	//移動
	Move(owner, input);
}

void PlayerStateMoving::ChangeWalkAnim(std::shared_ptr<Player> owner,Input& input)
{
	if (!m_isDash)
	{
		//スティックの割合からアニメーションを変更
		//小走り
		bool isJog = input.GetStickInfo().LeftStickRate() >= 0.5f;
		if (isJog)
		{
			owner->GetModel()->SetAnim(owner->GetAnim(L"Jog").c_str(), true);
		}
		else
		{
			owner->GetModel()->SetAnim(owner->GetAnim(L"Walk").c_str(), true);
		}
	}
}

void PlayerStateMoving::Move(std::shared_ptr<Player> owner, Input& input)
{
	//移動
	Vector3 vec = InputMoveVec(owner, input);
	float speed = m_speed;
	//ダッシュ中じゃないならスティックの割合
	if (!m_isDash)
	{
		speed *= input.GetStickInfo().LeftStickRate();
	}
	vec *= speed;
	owner->GetRb()->SetMoveVec(vec);

	//モデルの向き
	owner->GetModel()->SetDir(vec.XZ());
}
