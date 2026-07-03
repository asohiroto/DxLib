#include "PlayerStateFall.h"
#include "PlayerStateJump.h"
#include "PlayerStateIdle.h"
#include "PlayerStateHit.h"
#include "Player.h"
#include "PlayerStateMoving.h"
#include "PlayerStateAvoid.h"
#include "PlayerStateDeath.h"
#include "PlayerStateLightAttack.h"
#include "PlayerStateHeavyAttack.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"
#include "../../../../General/Sound/SoundManager.h"

namespace
{
	//アニメーション
	const std::wstring kFall = L"Fall";
	const std::wstring kGliding = L"PodFall";
	//着地SE
	const std::wstring kLanding = L"Jump";
	//減速率
	constexpr float kAirDecel = 0.9f;
	//空中加速度
	constexpr float kAirAccel = 2.0f;
	//壁に当たってる時の加速度倍率
	constexpr float kWallAccelRate = 0.1f;
}

PlayerStateFall::PlayerStateFall(std::weak_ptr<Actor> player, bool isWait) :
	PlayerStateBase(player,isWait),
	m_isGliding(false),
	m_isGlided(false)
{
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kFall).c_str(), true);
	owner->SetCollState(CollisionState::Fall);
}

PlayerStateFall::~PlayerStateFall()
{
}
void PlayerStateFall::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}
void PlayerStateFall::Update()
{
	auto& input = Input::GetInstance();
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());

	//強制待機
	if (m_isWait)
	{
		//待機
		ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner, m_isWait));
		return;
	}
	//ステータス
	auto status = owner->GetCharaStatus();
	//回避
	if (input.IsBuffered("B") && owner->IsAvoidable())
	{
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
	//地面に付いたら
	if (owner->IsFloor())
	{
		//SE再生
		SoundManager::GetInstance().PlayOnceSE(owner->GetSEPath(kLanding));

		ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner, m_isWait));
		return;
	}
	//ジャンプ
	if (owner->IsJumpable() && input.IsBuffered("A"))
	{
		ChangeState(std::make_shared<PlayerStateJump>(m_pOwner, m_isWait));
		return;
	}

	//空中攻撃可能なら
	if (!owner->IsAirAttacked())
	{
		//攻撃
		if (input.IsBuffered("X"))
		{
			ChangeState(std::make_shared<PlayerStateLightAttack>(m_pOwner, m_isWait));
			return;
		}
		if (input.IsBuffered("Y"))
		{
			ChangeState(std::make_shared<PlayerStateHeavyAttack>(m_pOwner, m_isWait));
			return;
		}
	}

	//落下中の移動
	MoveFall(input, owner, status);
}

void PlayerStateFall::MoveFall(Input& input, std::shared_ptr<Player> owner, std::shared_ptr<CharaStatus> status)
{
	auto rb = owner->GetRb();
	//現在の移動速度
	Vector3 currentVel = rb->GetMoveVec();

	//移動の入力があるなら
	if (input.GetStickInfo().IsLeftStickInput())
	{
		//入力方向ベクトル
		Vector3 inputDir = InputMoveVec(owner, input);
		inputDir = inputDir.Normalize();

		//空中移動加速度
		Vector3 addForce = inputDir * kAirAccel;

		//壁に当たってたら
		if (owner->IsWall())
		{
			addForce *= kWallAccelRate; // 壁に当たっている場合は半分に
		}

		//今の速度に力を加える
		currentVel += addForce;

		//最大速度制限
		float maxSpeed = status->GetMS(); // ステータスに設定された移動速度を上限とする

		float horizontalSpeed = currentVel.Magnitude();
		if (horizontalSpeed > maxSpeed)
		{
			float ratio = maxSpeed / horizontalSpeed;
			currentVel.x *= ratio;
			currentVel.z *= ratio;
		}
		//更新
		rb->SetMoveVec(currentVel);

		//モデルの向き
		owner->GetModel()->SetDir(inputDir.XZ());
	}
	//ない場合は減速してく
	else
	{
		//空中移動
		rb->SpeedDown(kAirDecel);
	}
	if (input.IsPress("A") && !m_isGlided)
	{
		//滑空
		owner->GetModel()->SetAnim(owner->GetAnim(kGliding).c_str(), false);
		//落下速度を遅くする
		rb->SetVecY(rb->GetVec().y / 2.0f);
		//滑空中
		m_isGliding = true;
		
	}
	else
	{
		if (input.IsRelease("A"))
		{
			//滑空をした
			m_isGlided = true;
		}
		m_isGliding = false;
		//落下
		owner->GetModel()->SetAnim(owner->GetAnim(kFall).c_str(), true);
	}
}
