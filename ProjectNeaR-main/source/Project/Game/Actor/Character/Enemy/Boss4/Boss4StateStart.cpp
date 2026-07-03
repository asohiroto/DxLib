#include "Boss4StateStart.h"
#include "Boss4StateIdle.h"
#include "Boss4.h"
#include "../EnemyBase.h"
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
	const std::wstring kStart = L"Attack4";

	//ボイス
	const std::wstring kStartVoice = L"Start";

	//カメラ距離
	constexpr float kCameraDistance = 300.0f;
}

Boss4StateStart::Boss4StateStart(std::weak_ptr<Actor> enemy) :
	EnemyStateBase(enemy, false),
	m_isCreate(false)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	auto model = owner->GetModel();
	model->SetAnim(owner->GetAnim(kStart).c_str(), false);
	owner->SetCollState(CollisionState::Normal);

	//再生を止める
	model->SetAnimSpeed(0.0f);
}

Boss4StateStart::~Boss4StateStart()
{
}

void Boss4StateStart::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void Boss4StateStart::Update()
{
	//強制待機状態なら
	if (m_isWait)return;
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Boss4>(m_pOwner.lock());

	auto model = owner->GetModel();

	//再生
	model->SetAnimSpeed(1.0f);

	//カメラの作成
	if (!m_isCreate)
	{
		//カメラ作成
		CameraController::GetInstance().PushCamera(std::make_shared<BossCamera>(owner->GetCenterPos(), model->GetDir(),
			kCameraDistance, owner->GetActorManager(), true));

		//ボイス再生
		SoundManager::GetInstance().PlayVoice(owner->GetVoicePath(kStartVoice));

		m_isCreate = true;
	}

	//モデルのアニメーションが終わったら
	if (model->IsFinishAnim())
	{
		ChangeState(std::make_shared<Boss4StateIdle>(owner, m_isWait));
		return;
	}

	owner->GetRb()->SetMoveVec(Vector3::Zero());
}
