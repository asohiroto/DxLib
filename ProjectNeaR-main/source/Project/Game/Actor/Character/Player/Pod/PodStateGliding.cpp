#include "PodStateGliding.h"
#include "Pod.h"
#include "PodStateIdle.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/Input.h"

namespace
{
	const std::wstring kIdle = L"Idle";
	//位置
	constexpr float kPodPosRight = 0.0f;
	constexpr float kPodPosBack = 0.0f;
	constexpr float kPodPosUp = 80.0f;
	//線形補間
	constexpr float kLerpRate = 1.0f;
}

PodStateGliding::PodStateGliding(std::weak_ptr<Actor> pod):
	PodStateBase(pod)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Pod>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kIdle).c_str(), true);
}

PodStateGliding::~PodStateGliding()
{
}

void PodStateGliding::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void PodStateGliding::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Pod>(m_pOwner.lock());

	//待機状態に戻るか
	if (!owner->IsGliding())
	{
		//待機
		ChangeState(std::make_shared<PodStateIdle>(m_pOwner));
		return;
	}

	//プレイヤーの近くに移動
	Vector3 targetPos = owner->GetPlayerPos();

	//プレイヤーの向き
	Vector3 playerDir = owner->GetPlayerDir();

	//今の座標
	Vector3 nowPos = owner->GetPos();

	//座標
	Vector3 nextPos = Vector3::Lerp(nowPos, GetPodPos(targetPos, playerDir, kPodPosUp, kPodPosRight, kPodPosBack), kLerpRate);
	owner->GetRb()->SetVec(nextPos - nowPos);

	//向きをカメラに合わせる
	owner->GetModel()->SetDir(playerDir.XZ());
}
