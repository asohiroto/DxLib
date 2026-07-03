#include "PlayerStateLightAttack.h"
#include "Player.h"
#include "PlayerStateMoving.h"
#include "PlayerStateIdle.h"
#include "PlayerStateAvoid.h"
#include "PlayerStateHeavyAttack.h"
#include "PlayerStateJump.h"
#include "PlayerStateFall.h"
#include "PlayerStateHit.h"
#include "PlayerStateDeath.h"
#include "Weapon/Weapon.h"
#include "../Enemy/EnemyBase.h"
#include "../../../Attack/SwordAttack.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/Sound/SoundManager.h"

namespace
{
	const std::wstring kFirstGroundAttackName = L"MainAttack1";
	const std::wstring kChargeName = L"MainAttackCharge";
	const std::wstring kJumpAttackName = L"RisingAttack";
	const std::wstring kJustAttackName = L"JustAttackMain";
	constexpr float kChargeFrame = 20.0f;
}

PlayerStateLightAttack::PlayerStateLightAttack(std::weak_ptr<Actor> player, bool isWait, bool isJump, bool isJust):
	PlayerStateAttackBase(player,isWait),
	m_isJust(isJust)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	owner->SetCollState(CollisionState::Move);

	//リジッドボディ
	auto rb = owner->GetRb();

	//空中にいるなら
	if (!owner->IsFloor())
	{
		//重力を受けない
		rb->SetIsGravity(false);
		//縦の移動量をリセット
		rb->SetVecY(0.0f);
		//空中攻撃をした
		owner->SetIsAirAttacked(true);
	}

	//武器を持つ
	owner->HaveLightSword();

	//ジャスト回避
	if (m_isJust)
	{
		//攻撃データ取得
		m_attackData = owner->GetAttackData(kJustAttackName);
		//上昇
		rb->SetVecY(m_attackData->GetParam1());
		//この攻撃の場合重力を受ける
		rb->SetIsGravity(true);
		//透明
		owner->SetIsDraw(false);
	}
	//ジャンプ中なら
	else if (isJump)
	{
		//攻撃データ取得
		m_attackData = owner->GetAttackData(kJumpAttackName);
		//上昇
		rb->SetVecY(m_attackData->GetParam1());
		//この攻撃の場合重力を受ける
		rb->SetIsGravity(true);
		//空中攻撃をしたことはなし
		owner->SetIsAirAttacked(false);
	}
	else
	{
		//攻撃データ取得
		m_attackData = owner->GetAttackData(kFirstGroundAttackName);
	}
	//アニメーション
	owner->GetModel()->SetAnim(owner->GetAnim(m_attackData->GetAnimName()).c_str(), false);

	//アーマー
	owner->ChangeArmor(m_attackData->GetArmor());
}

PlayerStateLightAttack::~PlayerStateLightAttack()
{
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	//重力を受ける
	owner->GetRb()->SetIsGravity(true);
	//攻撃削除
	DeleteAttack();
	owner->GetCharaStatus()->SetIsNoDamage(false);

	//アーマーをもとに戻す
	owner->InitArmor();
}

void PlayerStateLightAttack::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void PlayerStateLightAttack::Update()
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

	auto& input = Input::GetInstance();
	//回避
	if (input.IsBuffered("B") && owner->IsAvoidable() && !m_isJust)
	{
		//回避
		ChangeState(std::make_shared<PlayerStateAvoid>(m_pOwner, m_isWait));
		return;
	}

	//死亡
	if (owner->GetCharaStatus()->IsDead())
	{
		ChangeState(std::make_shared<PlayerStateDeath>(m_pOwner, m_isWait));
		return;
	}

	//やられ
	if (owner->GetCharaStatus()->IsHitReaction())
	{
		ChangeState(std::make_shared<PlayerStateHit>(m_pOwner, m_isWait));
		return;
	}
	//フレームをカウント
	CountFrame();
	
	//武器
	if (owner->GetWeapon(AnimData::WeaponType::LightSword).expired())return;
	auto weapon = owner->GetWeapon(AnimData::WeaponType::LightSword).lock();

	//モデル
	auto model = owner->GetModel();

	//発生フレームになったら
	UpdateStartAttack(owner, weapon);

	//ジャストアタック中無敵
	if( m_isJust)
	{
		owner->GetCharaStatus()->SetIsNoDamage(true);
	}


	//長押ししているフレームをカウント
	if (input.IsPress("X") && m_attackData->GetAnimName() != kChargeName && owner->IsFloor())
	{
		m_chargeCountFrame += owner->GetTimeScale();
	}
	else
	{
		m_chargeCountFrame = 0.0f;
	}
	//多段ヒット攻撃中はキャンセル攻撃をしない
	if (!m_attackData->IsMultipleHit())
	{
		//キャンセルフレーム
		if ((model->GetTotalAnimFrame() - m_attackData->GetCancelFrame()) < m_frame)
		{
			//攻撃の条件
			bool isChargeAttack = m_chargeCountFrame >= kChargeFrame;
			bool isCombAttack = input.IsBuffered("X");

			//武器を持つ
			owner->HaveLightSword();

			//攻撃をするか
			if (isChargeAttack || isCombAttack)
			{
				NextAttack(isChargeAttack, owner, model);
				return;
			}
			//ジャンプ
			if (owner->IsJumpable() && input.IsBuffered("A"))
			{
				ChangeState(std::make_shared<PlayerStateJump>(m_pOwner, m_isWait));
				return;
			}
			//大剣攻撃
			if (input.IsBuffered("Y"))
			{
				ChangeState(std::make_shared<PlayerStateHeavyAttack>(m_pOwner, m_isWait));
				return;
			}
		}
	}
	//アニメーションが終了したら
	if (model->IsFinishAnim())
	{
		return ChangeToMoveOrIdle(owner,input);
	}
	//移動
	UpdateMove(owner, input, model);
	//攻撃位置の更新
	UpdateAttackPosition(owner, weapon);
	//ジャスト回避の上昇のリセット
	if (m_isJust && m_frame >= m_attackData->GetMoveFrame())
	{
		//縦の移動量をリセット
		owner->GetRb()->SetVecY(0.0f);
	}
}

void PlayerStateLightAttack::NextAttack(bool isChargeAttack, std::shared_ptr<Player> owner, std::shared_ptr<Model> model)
{
	//次の攻撃名
	auto nextName = m_attackData->GetNextAttackName();

	//チャージ攻撃をするなら
	if (isChargeAttack)
	{
		nextName = kChargeName;
	}
	//次の攻撃がない場合
	else if (nextName == L"None")
	{
		//空中にいないなら
		if (owner->IsFloor())
		{
			//最初の攻撃に戻る
			nextName = kFirstGroundAttackName;
		}
	}
	if (nextName != L"None")
	{
		//攻撃データ
		m_attackData = owner->GetAttackData(nextName);
		m_isAppearedAttack = false;

		//アニメーション
		model->SetAnim(owner->GetAnim(m_attackData->GetAnimName()).c_str(), false);

		//フレームリセット
		m_frame = 0.0f;

		//攻撃削除
		DeleteAttack();

		return;
	}
}

void PlayerStateLightAttack::UpdateStartAttack(std::shared_ptr<Player>& owner, std::shared_ptr<Weapon>& weapon)
{
	if (m_frame >= m_attackData->GetStartFrame())
	{
		//持続が切れたら
		if (m_isAppearedAttack && m_pAttack.expired())
		{
			//多段ヒット攻撃の処理
			if (m_attackData->IsMultipleHit() && m_attackData->GetNextAttackName() != L"None")
			{
				//多段ヒット攻撃
				LoadNextMultipleHitAttack(owner,weapon);
				return;
			}
			//ジャスト回避攻撃の透明になるを解除
			if (m_isJust)
			{
				owner->SetIsDraw(true);
			}
			//重力を受ける
			owner->GetRb()->SetIsGravity(true);
		}
		else
		{
			//持続中は空中なら落下しない
			//切り上げ攻撃とジャスト回避攻撃以外
			if (!owner->IsFloor() && 
				!(m_attackData->GetAnimName() == kJumpAttackName || m_isJust))
			{
				//重力を受けない
				owner->GetRb()->SetIsGravity(false);
				owner->GetRb()->SetVecY(0.0f);
			}
			else
			{
				//重力を受ける
				owner->GetRb()->SetIsGravity(true);
				//空中攻撃をした
				owner->SetIsAirAttacked(true);
			}
		}

		//まだ攻撃が発生していないなら発生
		if (!m_isAppearedAttack)
		{
			CreateAttack(owner, weapon);
		}
	}
}

void PlayerStateLightAttack::ChangeToMoveOrIdle(std::shared_ptr<Player> owner, Input& input)
{
	//空中にいるなら落下
	if (!owner->IsFloor())
	{
		ChangeState(std::make_shared<PlayerStateFall>(m_pOwner, m_isWait));
		return;
	}
	if (input.GetStickInfo().IsLeftStickInput())
	{
		ChangeState(std::make_shared<PlayerStateMoving>(m_pOwner, m_isWait, false));
	}
	else
	{
		ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner, m_isWait));
	}
}
