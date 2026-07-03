#include "EnemyStateIdle.h"
#include "EnemyStateHit.h"
#include "EnemyStateAttack.h"
#include "EnemyStateMoving.h"
#include "EnemyStateFall.h"
#include "EnemyStateDeath.h"
#include "EnemyBase.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"

namespace
{
	//アニメーション
	const std::wstring kIdle = L"Idle";
}

EnemyStateIdle::EnemyStateIdle(std::weak_ptr<Actor> enemy, bool isWait) :
	EnemyStateBase(enemy,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kIdle).c_str(), true);
	owner->SetCollState(CollisionState::Normal);
}

EnemyStateIdle::~EnemyStateIdle()
{
}
void EnemyStateIdle::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}
void EnemyStateIdle::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());	

	//ステータス
	auto status = owner->GetCharaStatus();

	//強制待機状態ではないなら
	if (!m_isWait)
	{
		//死亡
		if (status->IsDead())
		{
			ChangeState(std::make_shared<EnemyStateDeath>(m_pOwner,m_isWait));
			return;
		}
		//ヒット状態
		if (owner->GetCharaStatus()->IsHitReaction())
		{
			//ヒット状態ならヒットステートへ
			ChangeState(std::make_shared<EnemyStateHit>(m_pOwner, m_isWait));
			return;
		}
		//落下
		if (!owner->IsFloor())
		{
			ChangeState(std::make_shared<EnemyStateFall>(m_pOwner, m_isWait, false));
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
				ChangeState(std::make_shared<EnemyStateAttack>(m_pOwner, m_isWait, attackData));
				return;
			}
		}
		//移動状態へ
		if (owner->GetTargetInfo().m_isFound)
		{
			if (!owner->IsInMeleeRange())
			{
				ChangeState(std::make_shared<EnemyStateMoving>(m_pOwner, m_isWait));
				return;
			}
		}
		//プレイヤーの方向を向く
		owner->UpdateModelDirToTargetDir();
	}

	//移動量リセット
	owner->GetRb()->SetMoveVec(Vector3::Zero());
}