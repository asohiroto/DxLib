#include "BossCamera.h"
#include "CameraController.h"
#include "../Actor/ActorManager.h"
#include "../UI/UIManager.h"
#include "../../General/Input.h"

namespace
{
	//ニアファー
	constexpr float kNear = 10.0f;
	constexpr float kFar = 20000.0f;
	//Lerp
	constexpr float kStartLerp = 0.05f;
	constexpr float kDeathLerp = 0.5f;

	//終了フレーム
	constexpr int kPopStartFrame = 120;
	constexpr int kPopDeathFrame = 300;
	//揺れ終了フレーム
	constexpr int kShakeFrame = 120;
	//揺れ強度
	constexpr int kShakePower = 250;
}

BossCamera::BossCamera(Vector3 bossPos, Vector3 bossDir, float distance, std::weak_ptr<ActorManager> pActorManager, bool isStart) :
	CameraBase(),
	m_bossPos(bossPos),
	m_bossDir(bossDir),
	m_pActorManager(pActorManager),
	m_countFrame(0),
	m_isStart(isStart)
{
	m_distance = distance;
}

BossCamera::~BossCamera()
{
	//入力を開始
	Input::GetInstance().StartUpdate();
	//UI表示
	UIManager::GetInstance().SetIsDraw(true);

	//キャラクターを行動可能状態に
	if (m_pActorManager.expired())return;
	m_pActorManager.lock()->AllOperate();
}

void BossCamera::Init()
{
	//値の初期化
	m_countFrame = 0;
	m_vertexAngle = 0.0f;
	m_front = Vector3::Forward();
	m_right = Vector3::Right();
	if (m_bossDir.SqMagnitude() > 0.0f)
	{
		m_bossDir = m_bossDir.Normalize();
	}
	m_look = m_bossDir * -1;
	m_rotH = Quaternion::IdentityQ();
	m_viewPos = m_bossPos;
	m_cameraPos = CameraController::GetInstance().GetBaseCameraPos();

	//死亡カメラの場合はカメラを揺らす
	if (!m_isStart)
	{
		CameraShake(kShakeFrame, kShakePower);
	}

	//キャラクターを待機状態に
	if (m_pActorManager.expired())return;
	m_pActorManager.lock()->AllWait();
}

void BossCamera::Update()
{
	//ニアファー
	SetCameraNearFar(kNear, kFar);

	//入力を停止
	Input::GetInstance().StopUpdate();
	//UI非表示
	UIManager::GetInstance().SetIsDraw(false);

	if (m_isStart)
	{
		Vector3 nextPos = m_bossPos + m_bossDir * m_distance;
		m_cameraPos = Vector3::Lerp(m_cameraPos, nextPos, kStartLerp);

		//反映 
		DxLib::SetCameraPositionAndTarget_UpVecY(
			m_cameraPos.ToDxLibVector(),
			m_viewPos.ToDxLibVector()
		);
		//スタートカメラ終了
		if (m_countFrame >= kPopStartFrame)
		{
			CameraController::GetInstance().PopCamera();
			return;
		}
	}
	else
	{
		Vector3 nextPos = m_bossPos + m_bossDir * m_distance;
		m_cameraPos = Vector3::Lerp(m_cameraPos, nextPos, kDeathLerp);

		//反映 
		DxLib::SetCameraPositionAndTarget_UpVecY(
			m_cameraPos.ToDxLibVector(),
			m_viewPos.ToDxLibVector()
		);
		//カメラの揺れ
		UpdateCameraShake();
		//死亡カメラ終了
		if (m_countFrame >= kPopDeathFrame)
		{
			CameraController::GetInstance().PopCamera();
			return;
		}
	}
	++m_countFrame;
}

void BossCamera::Draw() const
{

}

