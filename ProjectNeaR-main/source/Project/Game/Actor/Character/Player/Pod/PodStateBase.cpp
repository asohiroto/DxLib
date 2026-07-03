#include "PodStateBase.h"
#include "Pod.h"
#include "../../Enemy/EnemyBase.h"

PodStateBase::PodStateBase(std::weak_ptr<Actor> pod):
	CharacterStateBase(pod,false)
{
}

PodStateBase::~PodStateBase()
{
}

Vector3 PodStateBase::GetPodPos(Vector3 targetPos, Vector3 dir, float upH, float right, float back)
{
	//ターゲットの近くに移動
	targetPos.y += upH;
	//カメラに対して右
	Vector3 rightDir = dir.Cross(Vector3::Up());
	if (rightDir.SqMagnitude() > 0.0f)
	{
		rightDir = rightDir.Normalize();
	}
	targetPos += rightDir * right + dir * back;
	//座標
	Vector3 nextPos = targetPos;
	return nextPos;
}

Vector3 PodStateBase::GetPodDir(std::shared_ptr<Pod> owner)
{
	Vector3 dir = owner->GetCameraDir();
	//ターゲットがいるならターゲット方向を見る
	if (owner->GetPlayerTargetInfo().m_isFound)
	{
		if (owner->GetPlayerTargetInfo().m_pTarget.expired())return dir;
		auto target = owner->GetPlayerTargetInfo().m_pTarget.lock();
		if(target->GetGameTag() == GameTag::Enemy)
		{
			Vector3 toTarget = std::dynamic_pointer_cast<EnemyBase>(target)->GetLockOnViewPos() - owner->GetPos();
			if (toTarget.SqMagnitude() > 0.0f)
			{
				dir = toTarget.Normalize();
			}
		}
	}
	return dir;
}
