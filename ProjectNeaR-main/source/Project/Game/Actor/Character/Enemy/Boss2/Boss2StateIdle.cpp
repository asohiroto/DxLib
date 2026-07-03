#include "Boss2StateIdle.h"
#include "Boss2StateAttack.h"
#include "../EnemyStateAttack.h"
#include "../EnemyStateDeath.h"
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

Boss2StateIdle::Boss2StateIdle(std::weak_ptr<Actor> enemy,bool isWait) :
	EnemyStateBase(enemy,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kIdle).c_str(), true);
	owner->SetCollState(CollisionState::Normal);
}

Boss2StateIdle::~Boss2StateIdle()
{
}
void Boss2StateIdle::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}
void Boss2StateIdle::Update()
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
		//攻撃可能なら
		if (owner->IsEnableAttack())
		{
			//攻撃データ取得(距離から)
			auto attackData = owner->GetAttackByDistance();
			if (attackData)
			{
				//攻撃状態へ
				ChangeState(std::make_shared<Boss2StateAttack>(m_pOwner,m_isWait, attackData));
				return;
			}
		}
	}

	//移動量リセット
	owner->GetRb()->SetMoveVec(Vector3::Zero());
}