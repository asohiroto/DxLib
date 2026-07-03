#include "Boss2StateStart.h"
#include "Boss2StateIdle.h"
#include "Boss2.h"
#include "../EnemyBase.h"
#include "../../../ActorManager.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/Input.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/CharaStatus.h"
#include "../../../../../General/Effect/EffekseerManager.h"
#include "../../../../../General/Sound/SoundManager.h"
#include "../../../../Camera/BossCamera.h"
#include "../../../../Camera/CameraController.h"
namespace
{
	//アニメーション
	const std::wstring kStart = L"Shot";

	//ボイス
	const std::wstring kStartVoice = L"Start";

	//カメラ距離
	constexpr float kCameraDistance = 4000.0f;
}

Boss2StateStart::Boss2StateStart(std::weak_ptr<Actor> enemy) :
	EnemyStateBase(enemy, false),
	m_isCreate(false)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	owner->GetModel()->SetAnim(owner->GetAnim(kStart).c_str(), false);
	owner->SetCollState(CollisionState::Normal);
}

Boss2StateStart::~Boss2StateStart()
{
}

void Boss2StateStart::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void Boss2StateStart::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Boss2>(m_pOwner.lock());

	auto model = owner->GetModel();

	if (!m_isCreate)
	{
		if (!owner->GetActorManager().expired())
		{
			auto actorManager = owner->GetActorManager().lock();

			//カメラの作成
			CameraController::GetInstance().PushCamera(std::make_shared<BossCamera>(owner->GetCenterPos(), model->GetDir(),
				kCameraDistance, actorManager, true));

			//ボス戦
			actorManager->SetIsBossBattle(true);

			//ボイス再生
			SoundManager::GetInstance().PlayVoice(owner->GetVoicePath(kStartVoice));
		}
		m_isCreate = true;
	}

	//モデルのアニメーションが終わったら
	if (model->IsFinishAnim())
	{
		ChangeState(std::make_shared<Boss2StateIdle>(owner, m_isWait));
		return;
	}

	owner->GetRb()->SetMoveVec(Vector3::Zero());
}
