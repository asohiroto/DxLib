#include "Boss2StateAttack.h"
#include "Boss2.h"
#include "Boss2StateIdle.h"
#include "../EnemyStateDeath.h"
#include "../../Player/Player.h"
#include "../../../../Attack/AttackBase.h"
#include "../../../../Attack/SwordAttack.h"
#include "../../../../Attack/AOEAttack.h"
#include "../../../../Attack/EnemyBulletAttack.h"
#include "../../../../../General/Math/MyMath.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/CSV/AttackData.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/Sound/SoundManager.h"

namespace
{
	//弾のばらつき
	constexpr int kBulletAngle = 10;
	//トリガー判定に変換
	constexpr int kTriggerFrame = 280;
}

Boss2StateAttack::Boss2StateAttack(std::weak_ptr<Actor> enemy, bool isWait, std::shared_ptr<AttackData> attackData):
	EnemyStateAttack(enemy,isWait,attackData)
{
}

Boss2StateAttack::~Boss2StateAttack()
{
	//判定無効
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Boss2>(m_pOwner.lock());
	owner->EnableTriggerArm();
}

void Boss2StateAttack::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());

	//判定有効
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Boss2>(m_pOwner.lock());
	owner->DisableTriggerArm();
}

void Boss2StateAttack::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Boss2>(m_pOwner.lock());

	//強制待機状態へ
	if (m_isWait)
	{
		ChangeState(std::make_shared<Boss2StateIdle>(m_pOwner,m_isWait));
		return;
	}

	//ステータス
	auto status = owner->GetCharaStatus();
	//死亡
	if (status->IsDead())
	{
		ChangeState(std::make_shared<EnemyStateDeath>(m_pOwner, m_isWait));
		return;
	}

	//フレームをカウント
	CountFrame();

	//モデル
	auto model = owner->GetModel();

	//フレームによる攻撃の処理
	UpdateAttackFrame(owner);
	//攻撃移動更新
	UpdateMove(owner, model);
	//攻撃位置更新
	UpdateAttackPos(owner);

	//終了数フレーム前にトリガー判定
	if (m_frame >= kTriggerFrame)
	{
		owner->EnableTriggerArm();
	}

	//アニメーションが終了したら
	if (model->IsFinishAnim())
	{
		//待機
		ChangeState(std::make_shared<Boss2StateIdle>(m_pOwner, m_isWait));
		return;
	}
}


void Boss2StateAttack::CreateAttack(std::shared_ptr<EnemyBase> owner)
{
	if (!m_attackData)return;

	//SE再生
	SoundManager::GetInstance().PlayOnceSE(owner->GetSEPath(m_attackData->GetSEAppearPath()));

	std::shared_ptr<AttackBase> attack;

	//腕による攻撃の場合
	int armIndex = m_attackData->GetParam4();
	//攻撃が発動するかをチェック
	bool isAttackable = true;
	//左腕
	if (armIndex == static_cast<int>(Boss2::ArmIndex::LeftArm))
	{
		//腕があるなら攻撃可能
		isAttackable = !std::dynamic_pointer_cast<Boss2>(owner)->IsDestroyedLeftArm();
	}
	//右腕
	else if (armIndex == static_cast<int>(Boss2::ArmIndex::RightArm))
	{
		//腕があるなら攻撃可能
		isAttackable = !std::dynamic_pointer_cast<Boss2>(owner)->IsDestroyedRightArm();
	}

	//攻撃できるなら
	if (isAttackable)
	{
		//攻撃作成
		if (m_attackData->GetAttackType() == AttackData::AttackType::Sword)
		{
			attack = std::make_shared<SwordAttack>(m_attackData, owner);
		}
		else if (m_attackData->GetAttackType() == AttackData::AttackType::AOE)
		{
			attack = std::make_shared<AOEAttack>(m_attackData, owner);
		}
		else if (m_attackData->GetAttackType() == AttackData::AttackType::Bullet)
		{
			attack = std::make_shared<EnemyBulletAttack>(m_attackData, owner);
			//弾の初期位置と方向を設定
			auto bulletAttack = std::dynamic_pointer_cast<EnemyBulletAttack>(attack);
			auto model = owner->GetModel();

			//発射座標
			Vector3 bulletPos = MV1GetFramePosition(model->GetModelHandle(), static_cast<int>(m_attackData->GetParam1()));
			bulletAttack->SetPos(bulletPos);

			//モデルの向き
			Vector3 bulletDir = model->GetDir();

			//ターゲットがいるとき
			if (owner->GetTargetInfo().m_isFound)
			{
				auto player = std::dynamic_pointer_cast<Player>(owner->GetTargetInfo().m_pTarget.lock());
				Vector3 targetDir = player->GetCenterPos() - bulletPos;
				if (targetDir.SqMagnitude() > 0.0f)
				{
					targetDir = targetDir.Normalize();
				}
				bulletDir = targetDir;
			}
			//正規化
			if (bulletDir.SqMagnitude() > 0.0f)
			{
				bulletDir = bulletDir.Normalize();
			}
			//角度をランダムで回転
			bulletDir = Quaternion::AngleAxis(static_cast<float>(MyMath::GetRand(-kBulletAngle, kBulletAngle)) * MyMath::DEG_2_RAD, Vector3::Up().Cross(bulletDir)) * bulletDir;
			bulletDir = Quaternion::AngleAxis(static_cast<float>(MyMath::GetRand(-kBulletAngle, kBulletAngle)) * MyMath::DEG_2_RAD, Vector3::Up()) * bulletDir;

			//移動
			bulletAttack->SetMoveVec(bulletDir * m_attackData->GetParam2());

			//壊れるか
			bulletAttack->SetIsDestructible(m_attackData->GetParam3() != 0.0f);

			//弾を打った
			m_isShotBullet = true;
		}
		owner->SetAttack(attack);
		m_pAttack = attack;
	}
	m_isAppearedAttack = true;
}
