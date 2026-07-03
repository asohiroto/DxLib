#include "Boss3StateAttack.h"
#include "Boss3.h"
#include "Boss3StateIdle.h"
#include "Boss3StateHit.h"
#include "../EnemyStateDeath.h"
#include "../../Player/Player.h"
#include "../../../../Attack/AttackBase.h"
#include "../../../../Attack/SwordAttack.h"
#include "../../../../Attack/AOEAttack.h"
#include "../../../../Attack/EnemyBulletAttack.h"
#include "../../../../Attack/MissileAttack.h"
#include "../../../../Attack/BeamAttack.h"
#include "../../../../../General/Math/MyMath.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/CSV/AttackData.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/Effect/NormalEffect.h"
#include "../../../../../General/Effect/EffekseerManager.h"
#include "../../../../../General/Sound/SoundManager.h"

namespace
{
	//弾のばらつき
	constexpr int kBulletAngle = 10;
	//トリガー判定に変換
	constexpr int kTriggerFrame = 280;
	//ミサイルのばらつき
	constexpr float kMissileAngle = 30.0f;

	//ビームポインター
	const std::wstring kBeamPointer = L"BeamPointer";
}

Boss3StateAttack::Boss3StateAttack(std::weak_ptr<Actor> enemy, bool isWait, std::shared_ptr<AttackData> attackData) :
	EnemyStateAttack(enemy,isWait, attackData),
	m_pBeams(),
	m_pointerDir(Vector3::Zero())
{
	//この攻撃がビームなら
	if (m_attackData->GetAttackType() == AttackData::AttackType::Beam)
	{
		if (m_pOwner.expired())return;
		auto owner = std::dynamic_pointer_cast<Boss3>(m_pOwner.lock());
		//フレームインデックス取得
		int handle = owner->GetModel()->GetModelHandle();
		Vector3 pos = MV1GetFramePosition(handle, static_cast<int>(m_attackData->GetParam1()));
		m_pBeamPointer = EffekseerManager::GetInstance().CreateEffect(owner->GetEffectPath(kBeamPointer), pos);
		m_pointerDir = owner->GetModel()->GetDir();
	}
}

Boss3StateAttack::~Boss3StateAttack()
{
	if (!m_pBeams.empty())
	{
		for (auto& beam : m_pBeams)
		{
			if (beam.expired())continue;
			beam.lock()->Delete();
		}
	}
	if(!m_pBeamPointer.expired())
	{
		m_pBeamPointer.lock()->Delete();
	}
}

void Boss3StateAttack::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Boss3>(m_pOwner.lock());

	//強制待機状態へ
	if (m_isWait)
	{
		ChangeState(std::make_shared<Boss3StateIdle>(m_pOwner,m_isWait));
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
	//ヒット状態
	if (owner->GetCharaStatus()->IsHitReaction())
	{
		//ヒット状態ならヒットステートへ
		ChangeState(std::make_shared<Boss3StateHit>(m_pOwner, m_isWait));
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

	//ビーム攻撃の際にポインターを出す
	if (!m_pBeamPointer.expired())
	{
		auto beamPointer = m_pBeamPointer.lock();
		//向き
		if (m_pointerDir.SqMagnitude() > 0.0f)
		{
			m_pointerDir = m_pointerDir.Normalize();
		}
		//終点を決める
		m_pointerDir = Quaternion::AngleAxis(m_attackData->GetParam3() * MyMath::DEG_2_RAD, Vector3::Up()) * m_pointerDir;
		beamPointer->LookAt(m_pointerDir);
		beamPointer->SetTimeScale(owner->GetTimeScale());
	}

	
	//アニメーションが終了したら
	if (model->IsFinishAnim())
	{
		//待機
		ChangeState(std::make_shared<Boss3StateIdle>(m_pOwner, m_isWait));
		return;
	}
}

void Boss3StateAttack::UpdateAttackFrame(std::shared_ptr<EnemyBase> owner)
{
	if (m_frame >= m_attackData->GetStartFrame())
	{
		//持続が切れたら
		if (m_isAppearedAttack && (m_pAttack.expired() ||
			m_attackData->GetAttackType() == AttackData::AttackType::Bullet ||
			m_attackData->GetAttackType() == AttackData::AttackType::Missile || 
			m_attackData->GetAttackType() == AttackData::AttackType::Beam))
		{
			//多段ヒット攻撃の処理
			if (m_attackData->IsMultipleHit() && m_attackData->GetNextAttackName() != L"None")
			{
				//多段ヒット攻撃
				ComboAttack(owner);
				return;
			}
			else
			{
				//アーマーをもとに戻す
				owner->InitArmor();
			}
		}
		//まだ攻撃が発生していないなら発生
		if (!m_isAppearedAttack)
		{
			CreateAttack(owner);
		}
	}
}


void Boss3StateAttack::CreateAttack(std::shared_ptr<EnemyBase> owner)
{
	if (!m_attackData)return;

	//SE再生
	SoundManager::GetInstance().PlayOnceSE(owner->GetSEPath(m_attackData->GetSEAppearPath()));

	std::shared_ptr<AttackBase> attack;
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

		//XZ方向はモデルの向き
		Vector3 bulletDir = model->GetDir();

		//正規化
		if (bulletDir.SqMagnitude() > 0.0f)
		{
			bulletDir = bulletDir.Normalize();
		}
		//発射角度
		bulletDir = Quaternion::AngleAxis(m_attackData->GetParam4() * MyMath::DEG_2_RAD, Vector3::Up()) * bulletDir;
		//縦方向の角度
		bulletDir = Quaternion::AngleAxis(std::abs(std::cosf(m_frame * MyMath::DEG_2_RAD)) * m_attackData->GetLength()
			* MyMath::DEG_2_RAD, bulletDir.Cross(Vector3::Up())) * bulletDir;

		//移動
		bulletAttack->SetMoveVec(bulletDir * m_attackData->GetParam2());

		//壊れるか
		bulletAttack->SetIsDestructible(m_attackData->GetParam3() != 0.0f);

		//弾を打った
		m_isShotBullet = true;
	}
	else if (m_attackData->GetAttackType() == AttackData::AttackType::Missile)
	{
		attack = std::make_shared<MissileAttack>(m_attackData, owner);
		//弾の初期位置と方向を設定
		auto bulletAttack = std::dynamic_pointer_cast<MissileAttack>(attack);
		auto model = owner->GetModel();

		//発射座標
		Vector3 bulletPos = MV1GetFramePosition(model->GetModelHandle(), static_cast<int>(m_attackData->GetParam1()));
		bulletAttack->SetPos(bulletPos);

		//移動
		Vector3 moveDir = Quaternion::AngleAxis(MyMath::GetRandF(-kMissileAngle, kMissileAngle) * MyMath::DEG_2_RAD, model->GetDir()) * Vector3::Up();
		bulletAttack->SetMoveVec(moveDir * m_attackData->GetParam2());

		//ターゲットがいるとき
		if (owner->GetTargetInfo().m_isFound)
		{
			auto player = std::dynamic_pointer_cast<Player>(owner->GetTargetInfo().m_pTarget.lock());
			//追尾ターゲット
			bulletAttack->SetTargetTracking(player, m_attackData->GetParam3(), m_attackData->GetParam4());
		}

		//弾を打った
		m_isShotBullet = true;
	}
	else if (m_attackData->GetAttackType() == AttackData::AttackType::Beam)
	{
		//ポインター削除
		if (!m_pBeamPointer.expired())
		{
			m_pBeamPointer.lock()->Delete();
		}

		attack = std::make_shared<BeamAttack>(m_attackData, owner);

		//ビームを回転
		auto beam = std::dynamic_pointer_cast<BeamAttack>(attack);
		//フレームインデックス取得
		int handle = owner->GetModel()->GetModelHandle();
		Vector3 pos1 = MV1GetFramePosition(handle, static_cast<int>(m_attackData->GetParam1()));
		//始点
		beam->SetStartPos(pos1);
		Vector3 dir = m_pointerDir;
		if (dir.SqMagnitude() > 0.0f)
		{
			dir = dir.Normalize();
		}
		//終点を決める
		dir = Quaternion::AngleAxis(m_attackData->GetParam2() * MyMath::DEG_2_RAD, Vector3::Up()) * dir;
		beam->SetEndPos(pos1 + (dir * m_attackData->GetLength()));

		//始点を軸に回転
		beam->SetRotaAngleAndAxis(m_attackData->GetParam3(), Vector3::Up());

		//ビームの参照
		m_pBeams.push_back(beam);
	}

	owner->SetAttack(attack);
	m_pAttack = attack;
	m_isAppearedAttack = true;
}


void Boss3StateAttack::UpdateMove(std::shared_ptr<EnemyBase> owner, std::shared_ptr<Model> model)
{
	if (!m_attackData)return;
	Vector3 moveVec = Vector3::Zero();

	//移動フレーム中は向きを更新
	if (m_frame < m_attackData->GetMoveFrame())
	{
		//向き
		Vector3 dir = owner->GetToTargetVec();
		//モデルの向き
		model->SetDir(dir.XZ());
	}
	//攻撃が出ている間移動
	if (!m_pAttack.expired())
	{
		//モデルの向きに移動
		moveVec = model->GetDir() * m_attackData->GetMoveSpeed();
	}

	//移動
	owner->GetRb()->SetMoveVec(moveVec);
}


void Boss3StateAttack::UpdateAttackPos(std::shared_ptr<EnemyBase> owner)
{
	if (!m_attackData)return;
	if (m_pAttack.expired())return;

	//フレームインデックス取得
	int handle = owner->GetModel()->GetModelHandle();
	Vector3 pos1 = MV1GetFramePosition(handle, static_cast<int>(m_attackData->GetParam1()));

	//攻撃の位置更新
	if (m_attackData->GetAttackType() == AttackData::AttackType::Sword ||
		m_attackData->GetAttackType() == AttackData::AttackType::Throw)
	{
		auto attack = std::dynamic_pointer_cast<SwordAttack>(m_pAttack.lock());
		attack->SetStartPos(pos1);
		Vector3 pos2 = MV1GetFramePosition(handle, static_cast<int>(m_attackData->GetParam2()));
		Vector3 dir = (pos2 - pos1);
		if (dir.SqMagnitude() > 0.0f)
		{
			dir = dir.Normalize();
		}
		attack->SetEndPos(pos1 + (dir * m_attackData->GetLength()));
	}
	else if (m_attackData->GetAttackType() == AttackData::AttackType::AOE)
	{
		auto attack = std::dynamic_pointer_cast<AOEAttack>(m_pAttack.lock());
		attack->SetPos(pos1);
	}
}
