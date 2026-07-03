#include "PodStateAttack.h"
#include "PodStateIdle.h"
#include "PodStateGliding.h"
#include "Pod.h"
#include "../../../../Attack/BulletAttack.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/Input.h"
#include "../../../../../General/CSV/AttackData.h"
#include "../../../../../General/Sound/SoundManager.h"
#include "../../../../../General/Effect/EffekseerManager.h"
#include "../../../../../General/Effect/TrackActorEffect.h"

namespace
{
	const std::wstring kAttackAnim = L"Attack";
	const std::wstring kAttackData = L"NormalShot";
	//位置
	constexpr float kPodPosRight = -100.0f;
	constexpr float kPodPosBack = -100.0f;
	constexpr float kPodPosUp = 80.0f;
	//線形補間
	constexpr float kLerpRate = 0.5f;
	//弾を打つ高さのオフセット
	constexpr float kShotPosYOffset = 30.0f;
	//弾のばらつき
	constexpr float kDispersion = 0.04f;

	//ショットエフェクト
	const std::wstring kShotEffect = L"Shot";
}

PodStateAttack::PodStateAttack(std::weak_ptr<Actor> pod):
	PodStateBase(pod)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Pod>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kAttackAnim).c_str(), true);
	//攻撃データ
	m_attackData = owner->GetAttackData(kAttackData);

	//発射エフェクト
	m_shotEffect = EffekseerManager::GetInstance().CreateTrackActorEffect(owner->GetEffectPath(kShotEffect), owner);

	//ボイス再生
	SoundManager::GetInstance().PlayVoice(owner->GetVoicePath(m_attackData->GetVoicePath()));
}

PodStateAttack::~PodStateAttack()
{
	if (m_shotEffect.expired())return;
	m_shotEffect.lock()->Delete();
}

void PodStateAttack::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void PodStateAttack::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Pod>(m_pOwner.lock());

	//強制待機状態
	if (m_isWait)
	{
		//待機
		ChangeState(std::make_shared<PodStateIdle>(m_pOwner));
		return;
	}

	//滑空状態
	if (owner->IsGliding())
	{
		//滑空
		ChangeState(std::make_shared<PodStateGliding>(m_pOwner));
		return;
	}

	auto& input = Input::GetInstance();
	if (!input.IsPress("RB"))
	{
		//待機
		ChangeState(std::make_shared<PodStateIdle>(m_pOwner));
		return;
	}

	//プレイヤーの近くに移動
	Vector3 targetPos = owner->GetPlayerPos();
	//カメラの向き
	Vector3 dir = GetPodDir(owner);

	//今の座標
	Vector3 nowPos = owner->GetPos();

	//座標
	Vector3 nextPos = Vector3::Lerp(nowPos, GetPodPos(targetPos, owner->GetCameraDir(), kPodPosUp, kPodPosRight, kPodPosBack), kLerpRate);
	owner->GetRb()->SetVec(nextPos - nowPos);

	//向きをカメラに合わせる
	owner->GetModel()->SetDir(dir.XZ());

	//エフェクトの向き
	if (!m_shotEffect.expired())
	{
		m_shotEffect.lock()->LookAt(dir);
	}

	//カウント
	CountFrame();

	//発生フレームになったら弾を打つ
	CreateAttack(owner, dir);
	
}

void PodStateAttack::CreateAttack(std::shared_ptr<Pod> owner, const Vector3& dir)
{
	if (m_attackData->GetStartFrame() <= m_frame)
	{
		//SE再生
		SoundManager::GetInstance().PlayOnceSE(owner->GetSEPath(m_attackData->GetSEAppearPath()));

		//弾を打つ
		auto bullets = owner->GetBullets();

		for (auto bullet : bullets)
		{
			//活動中でない弾があるなら
			if (!bullet->IsActive())
			{
				//Activeにしつつフラグ等のリセット
				bullet->Reset(static_cast<float>(m_attackData->GetKeepFrame()));
				//ポッドの少し上
				Vector3 bulletPos = owner->GetPos();
				bulletPos.y += kShotPosYOffset;
				bullet->SetPos(bulletPos);

				//向きは少し向きをばらつかせる
				Vector3 bulletDir = dir;
				bulletDir += Vector3(MyMath::GetRandF(-kDispersion, kDispersion), MyMath::GetRandF(-kDispersion, kDispersion), MyMath::GetRandF(-kDispersion, kDispersion));
				if (bulletDir.SqMagnitude() > 0.0f)
				{
					bulletDir = bulletDir.Normalize();
				}
				bullet->SetMoveVec(bulletDir * m_attackData->GetMoveSpeed());
				owner->SetAttack(bullet);
				break;
			}
		}
		m_frame = 0.0f;
	}
}
