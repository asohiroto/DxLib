#include "Boss4StateRandMoving.h"
#include "Boss4StateIdle.h"
#include "Boss4StateHit.h"
#include "Boss4StateAttack.h"
#include "Boss4StateDeath.h"
#include "Boss4.h"
#include "../EnemyStateAttack.h"
#include "../EnemyStateDeath.h"
#include "../EnemyStateHit.h"
#include "../EnemyBase.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/Input.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/CharaStatus.h"

namespace
{
	//アニメーション
	const std::wstring kBack = L"Back";

	//終了フレーム
	constexpr float kFinishMovingFrame = 200.0f;

	//第二形態の時速度2倍
	constexpr float kSecondPhaseSpeedRate = 2.0f;
}

Boss4StateRandMoving::Boss4StateRandMoving(std::weak_ptr<Actor> enemy, bool isWait) :
	EnemyStateBase(enemy, isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Boss4>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kBack).c_str(), true);
	owner->SetCollState(CollisionState::Normal);

	//ランダムな方向を取得
	m_randVec = Vector3::GetRandVecXZ();
	if (m_randVec.SqMagnitude() > 0.0f)
	{
		m_randVec = m_randVec.Normalize() * owner->GetCharaStatus()->GetMS();
	}

	//第二形態なら
	if (owner->IsSecondPhase())
	{
		//倍の速度に
		m_randVec *= kSecondPhaseSpeedRate;
	}
}

Boss4StateRandMoving::~Boss4StateRandMoving()
{
}
void Boss4StateRandMoving::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}
void Boss4StateRandMoving::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Boss4>(m_pOwner.lock());

	//ステータス
	auto status = owner->GetCharaStatus();

	//強制待機状態ではないなら
	if (!m_isWait)
	{
		//死亡
		if (status->IsDead())
		{
			ChangeState(std::make_shared<Boss4StateDeath>(m_pOwner, m_isWait));
			return;
		}
		//ヒット状態
		if (owner->GetCharaStatus()->IsHitReaction())
		{
			//ヒット状態ならヒットステートへ
			ChangeState(std::make_shared<Boss4StateHit>(m_pOwner, m_isWait));
			return;
		}
		//攻撃可能なら
		if (owner->IsEnableAttack())
		{
			//攻撃データ取得(距離から)
			auto attackData = owner->GetAttackByDistance();
			if (attackData)
			{
				//攻撃状態へ
				ChangeState(std::make_shared<Boss4StateAttack>(m_pOwner, m_isWait, attackData));
				return;
			}
		}
		//プレイヤーの方向を向く
		owner->UpdateModelDirToTargetDir();

		//移動終了フレーム
		if (m_frame > kFinishMovingFrame)
		{
			ChangeState(std::make_shared<Boss4StateIdle>(m_pOwner, m_isWait));
			return;
		}
		m_frame += owner->GetTimeScale();
	}

	//移動
	owner->GetRb()->SetMoveVec(m_randVec);

}