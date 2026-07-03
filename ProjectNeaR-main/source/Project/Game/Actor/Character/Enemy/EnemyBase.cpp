#include "EnemyBase.h"
#include "../../../../General/Collision/CapsuleCollider.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/Model.h"
#include "../Player/Player.h"
#include "../../../../General/MyDraw.h"
#include "../../../../General/Effect/EffekseerManager.h"
#include "../../../../General/Effect/NormalEffect.h"
#include "../../../../General/CSV/CSVDataLoader.h"
#include "../../../../General/CSV/CSVData.h"
#include "../../../../General/CSV/AttackData.h"
#include "../../../../General/CSV/EnemyAttackKeyData.h"
#include "../../../../General/CSV/CharaStatusData.h"
#include "../../ActorManager.h"

namespace
{
	//警戒状態なら索敵範囲を広げる
	constexpr float kAlertedDisRate = 2.0f;
	//目の数
	constexpr int kEyeNum = 2;

	//初期攻撃クールタイム
	constexpr float kFirstCoolTime = 60.0f;
}

EnemyBase::EnemyBase(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager):
	CharacterBase(actorData, charaStatusData, Shape::Capsule, pActorManager),
	m_attackCoolTime(kFirstCoolTime),
	m_isActive(true),
	m_isBoss(false),
	m_isAlerted(true),
	m_isInArea(false),
	m_rightEyeIndex(0),
	m_leftEyeIndex(0),
	m_nearEyeIndex(0),
	m_eyeEffect(),
	m_canAttack(true),
	m_isInGroup(false)
{
	//グループタグ
	m_groupTag = charaStatusData->GetGroupTag();
	//グループに所属しているか
	m_isInGroup = !(m_groupTag == L"None" || m_groupTag == L"");
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Update()
{
	//非活動中は更新しない
	if (!m_isActive)return;
	//攻撃を喰らった際
	if (m_charaStatus->IsHit())
	{
		m_model->ModelHit();
	}
	//攻撃クールタイム更新
	CountAttackCoolTime();
	//共通処理
	CharacterBase::Update();
	//ロックオンの位置
	UpdateLockOnViewPos();
	//目の位置更新
	UpdateEyeEffect();
}


void EnemyBase::Draw() const
{
	//非活動中は描画しない
	if (!m_isActive)return;
#if _DEBUG
	//衝突判定
	DrawCapsule3D(
		m_rb->m_pos.ToDxLibVector(),
		std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetEndPos().ToDxLibVector(),
		std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetRadius(),
		16,
		0x00ff00,
		0x00ff00,
		false
	);
	//座標
	auto pos = m_rb->m_pos;
	//視野角
	float viewingAngle = m_charaStatus->GetSearchAngle();
	//索敵距離
	float searchDistance = m_charaStatus->GetSearchRange();

	//警戒
	if (m_isAlerted)
	{
		//長く
		searchDistance *= kAlertedDisRate;
	}

	//索敵範囲
	MyDraw::Draw3DCircle(pos, searchDistance, 36, 0x0000ff);
	//見てる方向
	if (m_model)
	{
		auto forward = m_model->GetDir();
		forward = forward * searchDistance;
		//視野角
		auto viewDir1 = Quaternion::AngleAxis(viewingAngle / 2.0f, Vector3::Up()) * forward;
		auto viewDir2 = Quaternion::AngleAxis(-viewingAngle / 2.0f, Vector3::Up()) * forward;
		//描画
		DrawLine3D(pos.ToDxLibVector(), (pos + forward).ToDxLibVector(), 0xff0000);
		DrawLine3D(pos.ToDxLibVector(), (pos + viewDir1).ToDxLibVector(), 0xff0000);
		DrawLine3D(pos.ToDxLibVector(), (pos + viewDir2).ToDxLibVector(), 0xff0000);
	}
#endif
	if (!m_model)return;
	m_model->Draw();
}

void EnemyBase::DrawShadow() const
{
	//非活動中は描画しない
	if (!m_isActive)return;
	if (!m_model)return;
	m_model->Draw();
}


bool EnemyBase::IsEnableAttack() const
{
	//攻撃可能な条件を満たした場合true
	return m_targetInfo.m_isFound && m_attackCoolTime <= 0.0f && m_canAttack;
}

Vector3 EnemyBase::GetHeadPos() const
{
	Vector3 headPos = std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetEndPos();
	headPos.y += GetRadius();
	return headPos;
}


void EnemyBase::UpdateLockOnViewPos()
{
	m_lockOnViewPos = GetCenterPos();
}

void EnemyBase::SearchTarget(std::shared_ptr<Player> player)
{
	if (!player)return;

	//エリア内ならどこにいてもプレイヤーを発見する
	if (m_isInArea)
	{
		m_targetInfo.m_pTarget = player;
		m_targetInfo.m_isFound = true;
		m_isAlerted = true;
		return;
	}

	//前回の発見状態を保存
	m_isAlerted = m_targetInfo.m_isFound;
	//視野角内にいるかどうか
	m_targetInfo.m_isFound = false;
	//索敵距離
	float searchDis = m_charaStatus->GetSearchRange();

	//攻撃を喰らったら警戒する
	if (m_charaStatus->IsHit() || m_isAlerted)
	{
		//警戒
		m_isAlerted = true;
		//長く
		searchDis *= kAlertedDisRate;
	}

	//プレイヤーの位置
	auto targetPos = player->GetRb()->GetPos();

	//距離を確認
	auto myPos = m_rb->GetPos();
	auto dir = targetPos.XZ() - myPos.XZ();
	if (dir.Magnitude() <= searchDis)
	{
		//警戒状態なら無条件で発見
		if (m_isAlerted)
		{
			m_targetInfo.m_pTarget = player;
			m_targetInfo.m_isFound = true;
			return;
		}

		//視野角内にターゲットがいるか
		auto angle = abs(Vector2::GetRad(m_model->GetDir().XZ(), dir));
		if (angle <= (m_charaStatus->GetSearchAngle() / 2.0f))
		{
			m_targetInfo.m_pTarget = player;
			m_targetInfo.m_isFound = true;
			m_isAlerted = true;
		}
	}
}

Vector3 EnemyBase::GetToTargetVec() const
{
	if (!m_targetInfo.m_isFound || m_targetInfo.m_pTarget.expired())
	{
		return Vector3::Zero();
	}
	auto target = m_targetInfo.m_pTarget.lock();
	Vector3 toTarget = target->GetRb()->GetPos() - this->GetRb()->GetPos();
	if (toTarget.SqMagnitude() > 0.0f)
	{
		toTarget = toTarget.Normalize();
	}
	return toTarget;
}

void EnemyBase::UpdateModelDirToTargetDir()
{
	Vector2 dir = GetToTargetVec().XZ();
	if(dir.Magnitude() > 0.0f)
	{
		m_model->SetDir(dir.XZ());
	}
}

bool EnemyBase::IsInMeleeRange() const
{
	if (!m_targetInfo.m_isFound || m_targetInfo.m_pTarget.expired())
	{
		return false;
	}
	auto target = m_targetInfo.m_pTarget.lock();
	Vector3 toTarget = target->GetRb()->GetPos() - this->GetRb()->GetPos();
	float distance = toTarget.Magnitude();
	if (distance < m_charaStatus->GetMeleeAttackRange())
	{
		return true;
	}
	return false;
}

void EnemyBase::CountAttackCoolTime()
{
	if (m_attackCoolTime <= 0.0f)return;
	m_attackCoolTime -= GetTimeScale();
}

bool EnemyBase::IsEnableLongRangeAttack() const
{
	//遠距離攻撃キーがあり、かつ近接範囲外なら可能
	return (m_longRangeAttackKeys.size() > 0) && !IsInMeleeRange();
}

bool EnemyBase::IsEnableMeleeAttack() const
{
	//近接攻撃キーがあり、かつ近接範囲内なら可能
	return (m_meleeAttackKeys.size() > 0) && IsInMeleeRange();
}

std::shared_ptr<AttackData> EnemyBase::GetAttackByDistance() const
{
	//攻撃データ
	std::shared_ptr<AttackData> attackData;

	//遠距離攻撃可能なら
	if (IsEnableLongRangeAttack())
	{
		attackData = GetRandomAttack(m_longRangeAttackKeys);
	}
	//近接攻撃可能なら
	else if (IsEnableMeleeAttack())
	{
		attackData = GetRandomAttack(m_meleeAttackKeys);
	}

	return attackData;
}
std::shared_ptr<AttackData> EnemyBase::GetRandomAttack(std::vector<std::wstring> keys) const
{
	int index = MyMath::GetRand(0, static_cast<int>(keys.size()) - 1);
	return GetAttackData(keys[index]);
}


void EnemyBase::InitLightUpEyesEff()
{
	if (m_pActorManager.expired())return;
	auto actorM = m_pActorManager.lock();
	if (actorM->GetPlayer().expired())return;

	Vector3 effPos;

	//右目の位置
	Vector3 rightEyePos = MV1GetFramePosition(m_model->GetModelHandle(), m_rightEyeIndex);
	//左目の位置
	Vector3 leftEyePos = MV1GetFramePosition(m_model->GetModelHandle(), m_leftEyeIndex);
	//プレイヤーの位置
	Vector3 playerPos = actorM->GetPlayer().lock()->GetPos();

	//近いほうを採用
	float rightDis = (playerPos - rightEyePos).Magnitude();
	float leftDis = (playerPos - leftEyePos).Magnitude();

	if (rightDis > leftDis)
	{
		//左目採用
		effPos = leftEyePos;
		m_nearEyeIndex = m_leftEyeIndex;
	}
	else
	{
		//右目採用
		effPos = rightEyePos;
		m_nearEyeIndex = m_rightEyeIndex;
	}

	//エフェクトを作成
	m_eyeEffect = EffekseerManager::GetInstance().CreateEffect(GetEffectPath(L"EyeLight"), effPos);
}

float EnemyBase::GetRadius() const
{
	return std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetRadius();
}


void EnemyBase::InitAttackKey(CSVDataLoader& csvLoader, std::wstring path)
{
	auto oriAttackKeys = csvLoader.LoadCSV(path.c_str());
	for (auto& data : oriAttackKeys)
	{
		std::shared_ptr<EnemyAttackKeyData> attackKeyData = std::make_shared<EnemyAttackKeyData>(data);
		//近接攻撃
		if (attackKeyData->GetAttackRangeType() == EnemyAttackKeyData::AttackRangeType::Melee)
		{
			m_meleeAttackKeys.emplace_back(attackKeyData->GetAttackKeyName());
		}
		//遠距離攻撃
		else if (attackKeyData->GetAttackRangeType() == EnemyAttackKeyData::AttackRangeType::LongRange)
		{
			m_longRangeAttackKeys.emplace_back(attackKeyData->GetAttackKeyName());
		}
	}
}

void EnemyBase::InitEyeIndex(CSVDataLoader& csvLoader, std::wstring path)
{
	auto eyeDatas = csvLoader.LoadCSV(path.c_str());
	for (auto& eyeData : eyeDatas)
	{
		auto data = eyeData->GetData();
		if (data.size() < kEyeNum)continue;
		m_leftEyeIndex = std::stoi(data[0]);
		m_rightEyeIndex = std::stoi(data[1]);
	}
}

void EnemyBase::UpdateEyeEffect()
{
	if (m_eyeEffect.expired())return;
	m_eyeEffect.lock()->SetPos(MV1GetFramePosition(m_model->GetModelHandle(), m_nearEyeIndex));
}
