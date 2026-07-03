#include "PodStateIdle.h"
#include "PodStateAttack.h"
#include "PodStateGliding.h"
#include "Pod.h"
#include "../Player.h"
#include "../../Enemy/EnemyBase.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/Input.h"

namespace
{
	const std::wstring kIdle = L"Idle";
	//位置
	constexpr float kPodPosRight = -50.0f;
	constexpr float kPodPosBack = -50.0f;
	constexpr float kPodPosUp = 100.0f;
	constexpr float kSwayHeight = 10.0f;
	//線形補間
	constexpr float kLerpRate = 0.1f;
}

PodStateIdle::PodStateIdle(std::weak_ptr<Actor> pod):
	PodStateBase(pod),
	m_swayAngle(0.0f)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Pod>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kIdle).c_str(), true);
}

PodStateIdle::~PodStateIdle()
{
}

void PodStateIdle::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void PodStateIdle::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Pod>(m_pOwner.lock());

	//滑空状態
	if (owner->IsGliding())
	{
		//滑空
		ChangeState(std::make_shared<PodStateGliding>(m_pOwner));
		return;
	}

	auto& input = Input::GetInstance();
	
	if (input.IsBuffered("RB") && !m_isWait)
	{
		//射撃
		ChangeState(std::make_shared<PodStateAttack>(m_pOwner));
		return;
	}
	
	//プレイヤーの近くに移動
	Vector3 targetPos = owner->GetPlayerPos();
	//上下に揺れる
	m_swayAngle += 0.05f;
	if (m_swayAngle > MyMath::PI * 2.0f)
	{
		m_swayAngle -= MyMath::PI * 2.0f;
	}
	targetPos.y += kSwayHeight * std::cos(m_swayAngle);

	
	//カメラの向き
	Vector3 dir = GetPodDir(owner);

	//今の座標
	Vector3 nowPos = owner->GetPos();

	//座標
	Vector3 nextPos = Vector3::Lerp(nowPos, GetPodPos(targetPos, dir, kPodPosUp, kPodPosRight, kPodPosBack), kLerpRate);
	owner->GetRb()->SetVec(nextPos - nowPos);

	//向きをカメラに合わせる
	owner->GetModel()->SetDir(dir.XZ());
}
