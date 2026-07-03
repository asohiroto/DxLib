#include "Boss3StateIdle.h"
#include "Boss3StateAttack.h"
#include "Boss3StateHit.h"
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
	const std::wstring kIdle = L"Idle";
}

Boss3StateIdle::Boss3StateIdle(std::weak_ptr<Actor> enemy, bool isWait) :
	EnemyStateBase(enemy,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kIdle).c_str(), true);
	owner->SetCollState(CollisionState::Normal);
}

Boss3StateIdle::~Boss3StateIdle()
{
}
void Boss3StateIdle::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}
void Boss3StateIdle::Update()
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
			ChangeState(std::make_shared<EnemyStateDeath>(m_pOwner, m_isWait));
			return;
		}
		//ヒット状態
		if (owner->GetCharaStatus()->IsHitReaction())
		{
			//ヒット状態ならヒットステートへ
			ChangeState(std::make_shared<Boss3StateHit>(m_pOwner, m_isWait));
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
				ChangeState(std::make_shared<Boss3StateAttack>(m_pOwner, m_isWait, attackData));
				return;
			}
		}
		//プレイヤーの方向を向く
		owner->UpdateModelDirToTargetDir();
	}

	//移動量リセット
	owner->GetRb()->SetMoveVec(Vector3::Zero());
}