#include "PlayerTestScene.h"
#include <Dxlib.h>
#include  "../../General/Input.h"
#include "../SceneController.h"
#include "../../Game/Actor/ActorManager.h"
#include "../../Game/Attack/AttackManager.h"
#include "../../Game/Camera/CameraController.h"
#include "../../Game/Camera/PlayerCamera.h"
#include "../../General/Collision/Physics.h"
#include "../../General/CSV/ActorData.h"
#include "../../General/CSV/CharaStatusData.h"

PlayerTestScene::PlayerTestScene(SceneController& controller) :
	SceneBase(controller)
{
}

PlayerTestScene::~PlayerTestScene()
{
}

void PlayerTestScene::Init()
{
	//カメラ
	auto camera = std::make_shared<PlayerCamera>();
	auto& cameraController = CameraController::GetInstance();
	cameraController.Init();
	cameraController.ChangeCamera(camera);

	m_attackManager = std::make_shared<AttackManager>();
	m_attackManager->Init();

	//アクター
	m_actorManager = std::make_shared<ActorManager>();
	m_actorManager->Init();
	m_actorManager->CreateActorCSV(L"DebugScene",L"CharacterData");
	m_actorManager->CreateActorCSV(L"DebugScene",L"StageData");
	//カメラセット
	m_actorManager->SetPlayerCamera(camera);
	//攻撃マネージャーセット
	m_actorManager->SetAttackManager(m_attackManager);
}

void PlayerTestScene::Update()
{
	//更新
	m_actorManager->Update();
	m_attackManager->Update();
	CameraController::GetInstance().Update();
}

void PlayerTestScene::Draw()
{
#if _DEBUG
	//グリッド線
	for (int i = -10; i <= 10; i++)
	{
		DrawLine3D(VGet(-1000, 0, i * 100), VGet(1000, 0, i * 100), 0xff0000);
		DrawLine3D(VGet(i * 100, 0, -1000), VGet(i * 100, 0, 1000), 0x00ffff);
	}
#endif
	m_actorManager->Draw();
	m_attackManager->Draw();
}

void PlayerTestScene::End()
{
	m_actorManager->End();
	m_attackManager->End();
}

void PlayerTestScene::DebugDraw() const
{
#if _DEBUG
	DrawString(0, 0, L"Player Test Scene", 0xffffff);
	DrawString(0, 16, L"[D]キーで Debug Scene", 0xffffff);
#endif
}
