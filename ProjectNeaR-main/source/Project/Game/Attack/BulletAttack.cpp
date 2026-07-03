#include "BulletAttack.h"
#include "../../General/Collision/SphereCollider.h"
#include "../../General/Collision/Rigidbody.h"
#include "../../General/CSV/AttackData.h"
#include "../Actor/Character/CharacterBase.h"
#include "../../General/Effect/EffekseerManager.h"
#include "../../General/Effect/NormalEffect.h"

BulletAttack::BulletAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner):
	SphereAttackBase(attackData,pOwner),
	m_isActive(true),
	m_moveVec(Vector3::Zero())
{
}

BulletAttack::~BulletAttack()
{
}

void BulletAttack::Update()
{
	//活動中かどうかで当たり判定を行うかを決める
	m_isThrough = !m_isActive;

	//AttackBase::Update()でfalseになってしまうのでここで結果を保持
	bool isHit = m_isHit;

	//攻撃基底の更新（ここで寿命カウントが進む）
	AttackBase::Update();
	//球の更新
	SphereAttackBase::Update();

	//非アクティブ化条件をまとめる
	bool shouldDeactivate = isHit || (m_keepFrame <= 0.0f);

	if (!m_attackEffect.expired())
	{
		auto eff = m_attackEffect.lock();
		if (shouldDeactivate)
		{
			eff->DisableDraw();//描画しない
		}
		else
		{
			//移動
			UpdateMove();
			//エフェクトの更新
			eff->SetPos(m_rb->GetPos());
			//描画する
			eff->EnableDraw();
		}
	}
	if (shouldDeactivate)
	{
		m_keepFrame = 0.0f;
		m_isActive = false;
		return;
	}
}


void BulletAttack::Draw() const
{
#if _DEBUG
	if (!m_isActive)return;
	auto coll = std::dynamic_pointer_cast<SphereCollider>(m_collisionData);
	DrawSphere3D(m_rb->m_pos.ToDxLibVector(),
		coll->GetRadius(), 16, 0xffc800, 0xffc800, false);
#endif
}

void BulletAttack::OnCollide(const std::shared_ptr<Collidable> other)
{
	if (other->GetGameTag() == GameTag::Object)
	{
		//当たったフラグをtrueに
		m_isHit = true;
		return;
	}
	AttackBase::OnCollide(other);
}

void BulletAttack::Reset(float keepFrame)
{
	//持続のリセット
	m_keepFrame = keepFrame;
	//当たったフラグリセット
	m_isHit = false;
	//当たったことのある対象をリセット
	ResetHitID();
	//活動
	m_isActive = true;
}


void BulletAttack::UpdateMove()
{
	//移動処理
	m_rb->SetVec(m_moveVec);
	m_rb->SetPos(m_rb->GetNextPos());
	m_rb->SetVec(Vector3::Zero());
}
