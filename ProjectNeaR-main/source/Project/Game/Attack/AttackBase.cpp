#include "AttackBase.h"
#include "../Actor/Character/CharacterBase.h"
#include "../Actor/Character/Enemy/EnemyBase.h"
#include "../../General/Collision/CapsuleCollider.h"
#include "../../General/Collision/SphereCollider.h"
#include "../../General/Collision/TorusCollider.h"
#include "../../General/CharaStatus.h"
#include "../../General/Collision/Rigidbody.h"
#include "../../General/Effect/EffekseerManager.h"
#include "../../General/Effect/NormalEffect.h"
#include "../../General/Model.h"
#include "../../General/Sound/SoundManager.h"
#include "../../Main/Application.h"

AttackBase::AttackBase(Shape shape, std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner) :
	Collidable(shape),
	m_pOwner(pOwner),
	m_oriPos(Vector3::Zero()),
	m_attackPower(0),
	m_attackWeight(CharaStatus::AttackWeight::Light),
	m_knockBackPower(0.0f),
	m_knockBackV(0.0f),
	m_keepFrame(0.0f),
	m_originPosType(AttackData::AttackOriginPosType::Center),
	m_hitStopFrame(0),
	m_hitStopShakePower(0),
	m_hitEffectPath(L""),
	m_attackEffectPath(L""),
	m_hitSEPath(L""),
	m_attackEffect(),
	m_isHit(false),
	m_isRequestHitStop(false)
{
	if (attackData)
	{
		m_attackPower = attackData->GetAttackPower();
		m_attackWeight = attackData->GetAttackWeight();
		m_knockBackPower = attackData->GetKnockBackPower();
		m_knockBackV = attackData->GetVerticalPower();
		m_keepFrame = attackData->GetKeepFrame();
		m_originPosType = attackData->GetAttackOriginPosType();
		m_hitStopFrame = attackData->GetHitStopFrame();
		m_hitStopShakePower = attackData->GetHitStopShakePower();
		m_hitEffectPath = attackData->GetHitEffectPath();
		m_attackEffectPath = attackData->GetAttackEffectPath();
		m_hitSEPath = attackData->GetSEHitPath();
		m_isHit = false;
		m_isRequestHitStop = false;
	}

	//コライダーの設定
	switch (shape)
	{
	case Shape::Sphere:
	{
		auto coll = std::dynamic_pointer_cast<SphereCollider>(m_collisionData);
		coll->SetRadius(attackData->GetRadius());
		break;
	}
	case Shape::Capsule:
	{
		auto coll = std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData);
		coll->SetRadius(attackData->GetRadius());
		break;
	}
	case Shape::Torus:
	{
		auto coll = std::dynamic_pointer_cast<TorusCollider>(m_collisionData);
		coll->SetRadius(attackData->GetRadius());
		coll->SetRange(attackData->GetLength());
		break;
	}
	default:
		break;
	};
	AllSetting(Priority::Static,GameTag::Attack, false,true, false);
}

AttackBase::~AttackBase()
{
	if (m_attackEffect.expired())return;
	m_attackEffect.lock()->Delete();
}

void AttackBase::Init()
{
	//登録
	Collidable::Init();

	//攻撃エフェクトを出す
	if (m_attackEffectPath != L"" &&
		m_attackEffectPath != L"None")
	{
		if (m_pOwner.expired())return;
		auto owner = m_pOwner.lock();

		auto attackEffect = EffekseerManager::GetInstance().CreateEffect(m_attackEffectPath, m_oriPos);
		if (attackEffect.expired())return;
		attackEffect.lock()->LookAt(owner->GetModel()->GetDir());
		m_attackEffect = attackEffect;
	}
}

void AttackBase::Update()
{
	//持続フレームを減らす
	m_keepFrame -= Application::GetInstance().GetTimeScale();
	//持続フレームが0になったら削除
	if (m_keepFrame < 0 || m_pOwner.expired())
	{
		return; //何もしない
	}
	//ヒットリセット
	m_isHit = false;
	m_isRequestHitStop = false;
	//エフェクト位置更新
	if (m_attackEffect.expired())return;
	m_attackEffect.lock()->SetPos(m_rb->GetPos());
}

void AttackBase::OnCollide(const std::shared_ptr<Collidable> other)
{
	if (m_pOwner.expired())return;
	if (!other)return;

	auto ownerColl = m_pOwner.lock();

	auto otherTag = other->GetGameTag();
	//キャラクターなのかをチェック
	if (otherTag == GameTag::Player || otherTag == GameTag::Enemy)
	{
		//自分と同じ種類なら無視
		if (otherTag == ownerColl->GetGameTag())return;
		//敵なら
		if (otherTag == GameTag::Enemy)
		{
			//活動中でないなら攻撃は喰らわない
			if (!std::dynamic_pointer_cast<EnemyBase>(other)->IsActive())
			{
				return;
			}
		}

		auto otherColl = std::dynamic_pointer_cast<CharacterBase>(other);
		
		//ID
		int otherID = otherColl->GetID();

		//IDがすでに記録されているか確認
		if (IsHitID(otherID))
		{
			//すでに当たっている
			return; 
		}
		else
		{
			//まだ当たっていない
			//IDを覚える
			m_hitId.insert(otherID);

			//相手
			auto otherStatus = otherColl->GetCharaStatus();

			//攻撃力
			int at = ownerColl->GetCharaStatus()->GetAT();

			//ダメージを与える
			otherStatus->OnDamage(m_attackPower, at, m_attackWeight);

			//攻撃の座標
			Vector3 attackPos = m_oriPos;

			//のけぞるか
			if (m_rb && otherStatus->IsHitReaction())
			{
				//ノックバックを与える
				Vector3 knockBack = otherStatus->GetKnockBack(otherColl->GetNextPos(), attackPos, m_knockBackPower, m_knockBackV);
				otherColl->GetRb()->SetVec(knockBack);
			}
			//無敵じゃないなら
			if (!otherStatus->IsNoDamage())
			{
				//ヒットストップを行う
				m_isRequestHitStop = true;

				//ヒットエフェクトを出す
				if (m_hitEffectPath != L"" &&
					m_hitEffectPath != L"None")
				{
					Vector3 hitPos = otherColl->GetCenterPos();
					Vector3 toAttackDir = attackPos - hitPos;
					if (toAttackDir.SqMagnitude() > 0.0f)
					{
						toAttackDir = toAttackDir.Normalize();
					}
					hitPos += (toAttackDir * otherColl->GetRadius());
					EffekseerManager::GetInstance().CreateEffect(m_hitEffectPath, hitPos);
				}

				//ヒットSE
				if (m_hitSEPath != L"" &&
					m_hitSEPath != L"None")
				{
					SoundManager::GetInstance().PlayOnceSE(ownerColl->GetSEPath(m_hitSEPath));
				}
			}

			//当たった
			m_isHit = true;
		}
	}
}


void AttackBase::End()
{
	Collidable::End();
}

bool AttackBase::IsDelete()
{
	return m_keepFrame <= 0.0f;
}

void AttackBase::Delete()
{
	m_keepFrame = 0.0f;
}

void AttackBase::ResetHitID()
{
	if (m_hitId.size() <= 0) return;
	m_hitId.clear();
}

GameTag AttackBase::GetOwnerTag() const
{
	if (m_pOwner.expired())return GameTag::None;
	return m_pOwner.lock()->GetGameTag();
}

bool AttackBase::IsHitID(int otherID)
{
	return m_hitId.contains(otherID);
}