#include "EnemyStateMoving.h"
#include "EnemyStateIdle.h"
#include "EnemyStateHit.h"
#include "EnemyStateAttack.h"
#include "EnemyStateFall.h"
#include "EnemyStateDeath.h"
#include "EnemyBase.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"
#include "../../../../General/Sound/SoundManager.h"
#include "../../../../General/Sound/SE.h"

namespace
{
	//アニメーション
	const std::wstring kWalk = L"Walk";
	//SE
	const std::wstring kMoveSE = L"Move";
	//最低保証フレーム数
	constexpr int kMinFrame = 10;
}
EnemyStateMoving::EnemyStateMoving(std::weak_ptr<Actor> enemy, bool isWait):
	EnemyStateBase(enemy,isWait),
	m_speed(0.0f)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kWalk).c_str(), true);
	owner->SetCollState(CollisionState::Move);
	//速度
	auto status = owner->GetCharaStatus();
	m_speed = status->GetMS();

	//SE
	m_moveSE = SoundManager::GetInstance().PlayLoopSE(owner->GetSEPath(kMoveSE));
}

EnemyStateMoving::~EnemyStateMoving()
{
	if (m_moveSE.expired())return;
	m_moveSE.lock()->Delete();
}

void EnemyStateMoving::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void EnemyStateMoving::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());

	//強制待機状態へ
	if (m_isWait)
	{
		ChangeState(std::make_shared<EnemyStateIdle>(m_pOwner, m_isWait));
		return;
	}

	//フレームカウント
	CountFrame();
	//ステータス
	auto status = owner->GetCharaStatus();
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
		if(attackData)
		{
			//攻撃状態へ
			ChangeState(std::make_shared<EnemyStateAttack>(m_pOwner, m_isWait, attackData));
			return;
		}
	}
	//待機状態へ
	if (!owner->GetTargetInfo().m_isFound || 
		(owner->IsInMeleeRange() && m_frame >= kMinFrame))
	{
		ChangeState(std::make_shared<EnemyStateIdle>(m_pOwner, m_isWait));
		return;
	}
	//移動
	Move(owner);
}

void EnemyStateMoving::Move(std::shared_ptr<EnemyBase> owner)
{
	//移動
	if (!owner->GetTargetInfo().m_isFound || owner->GetTargetInfo().m_pTarget.expired()) return;
	Vector3 vec = owner->GetToTargetVec();
	vec *= m_speed;
	owner->GetRb()->SetMoveVec(vec);

	//モデルの向き
	if (m_speed > 0.0f)
	{
		//移動速度0の時
		owner->GetModel()->SetDir(vec.XZ());
	}
	else
	{
		//プレイヤーの方向を向く
		owner->UpdateModelDirToTargetDir();
	}
}
