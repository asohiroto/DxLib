#include "PlayerStateAttackBase.h"
#include "Weapon/Weapon.h"
#include "../Enemy/EnemyBase.h"
#include "../../../Attack/SwordAttack.h"
#include "../../../Attack/AOEAttack.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/Sound/SoundManager.h"
#include "Player.h"

namespace
{
	//移動量の倍率
	constexpr float kLowSpeedRate = 0.5f;
	constexpr float kHighSpeedRate = 3.0f;
	//距離の影響割合
	constexpr float kDistanceRate = 0.005f;
}

PlayerStateAttackBase::PlayerStateAttackBase(std::weak_ptr<Actor> player, bool isWait):
	PlayerStateBase(player,isWait),
	m_isAppearedAttack(false),
	m_attackData(),
	m_pAttack(),
	m_chargeCountFrame(0.0f)
{
}

PlayerStateAttackBase::~PlayerStateAttackBase()
{
	//攻撃の削除
	DeleteAttack();
}

void PlayerStateAttackBase::DeleteAttack()
{
	if (m_pAttack.expired())return;
	auto attack = m_pAttack.lock();
	attack->Delete();
}

void PlayerStateAttackBase::CreateAttack(std::shared_ptr<Player> owner, std::shared_ptr<Weapon> weapon)
{
	if (!m_attackData)return;

	auto& soundManager = SoundManager::GetInstance();
	//SE再生
	soundManager.PlayOnceSE(owner->GetSEPath(m_attackData->GetSEAppearPath()));

	//攻撃ボイス
	soundManager.PlayVoice(owner->GetVoicePath(m_attackData->GetVoicePath()));

	//攻撃作成
	std::shared_ptr<AttackBase> attack;
	
	if (m_attackData->GetAttackType() == AttackData::AttackType::Sword ||
		m_attackData->GetAttackType() == AttackData::AttackType::Throw)
	{
		attack = std::make_shared<SwordAttack>(m_attackData, owner);
	}
	else if (m_attackData->GetAttackType() == AttackData::AttackType::AOE)
	{
		attack = std::make_shared<AOEAttack>(m_attackData, owner);
	}
	owner->SetAttack(attack);
	m_pAttack = attack;
	m_isAppearedAttack = true;

	auto model = owner->GetModel();
	//投げもの攻撃の処理
	if (m_attackData->GetAttackType() == AttackData::AttackType::Throw)
	{
		//投げつつ回転
		weapon->ThrowAndRoll(
			m_attackData->GetParam1(), model->GetDir(),
			owner->GetPos(), static_cast<float>(m_attackData->GetKeepFrame()), m_attackData->GetParam2());
	}
	else
	{
		//回転終了
		weapon->FinisiThrowAndRoll();
	}

	//アーマー
	owner->ChangeArmor(m_attackData->GetArmor());
}

void PlayerStateAttackBase::UpdateAttackPosition(std::shared_ptr<Player> owner, std::shared_ptr<Weapon> weapon)
{
	if (!m_attackData)return;
	if (!weapon)return;
	if (m_pAttack.expired())return;
	//攻撃の位置更新
	if (m_attackData->GetAttackType() == AttackData::AttackType::Sword ||
		m_attackData->GetAttackType() == AttackData::AttackType::Throw)
	{
		auto attack = std::dynamic_pointer_cast<SwordAttack>(m_pAttack.lock());
		attack->SetStartPos(weapon->GetStartPos());
		attack->SetEndPos(weapon->GetEndPos(m_attackData->GetLength()));
	}
	else if (m_attackData->GetAttackType() == AttackData::AttackType::AOE)
	{
		auto attack = std::dynamic_pointer_cast<AOEAttack>(m_pAttack.lock());
		attack->SetPos(weapon->GetStartPos());
	}
}

void PlayerStateAttackBase::LoadNextMultipleHitAttack(std::shared_ptr<Player> owner, std::shared_ptr<Weapon> weapon)
{
	if (!m_attackData)return;
	// 多段ヒット処理
	m_attackData = owner->GetAttackData(m_attackData->GetNextAttackName());
	m_isAppearedAttack = false;
	//攻撃作成
	if (m_frame >= m_attackData->GetStartFrame())
	{
		CreateAttack(owner, weapon);
	}
}

void PlayerStateAttackBase::UpdateMove(std::shared_ptr<Player> owner, Input& input, std::shared_ptr<Model> model)
{
	if (!m_attackData)return;
	Vector3 moveVec = Vector3::Zero();
	if (m_frame < m_attackData->GetMoveFrame())
	{
		//速度
		float speed = m_attackData->GetMoveSpeed();

		//向き
		Vector3 dir = InputMoveVec(owner, input);

		//ターゲットがいるならターゲット方向へ向く
		if(owner->GetTargetInfo().m_isFound)
		{
			if(!owner->GetTargetInfo().m_pTarget.expired())
			{
				auto target = owner->GetTargetInfo().m_pTarget.lock();
				//ターゲットがいるならターゲット方向へ向く
				dir = (target->GetPos() - owner->GetPos());
				if (dir.SqMagnitude() > 0.0f)
				{
					dir = dir.Normalize();
				}

				//ターゲットとの距離が近いなら移動量を減らし遠いなら増やす
				float dis = (target->GetPos() - owner->GetPos()).Magnitude();
				speed *= MathSub::ClampFloat(dis * kDistanceRate, kLowSpeedRate, kHighSpeedRate);
			}
		}

		//モデルの向き
		model->SetDir(dir.XZ());

		if (!owner->GetTargetInfo().m_isFound)
		{
			dir = model->GetDir();
		}

		//移動量
		moveVec = dir * speed;
	}
	//移動
	owner->GetRb()->SetMoveVec(moveVec);
}
