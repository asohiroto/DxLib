#include "GameScene.h"
#include "ResultScene.h"
#include "GameoverScene.h"
#include "PauseScene.h"
#include <Dxlib.h>
#include  "../General/Input.h"
#include "SceneController.h"
#include "../Game/Actor/ActorManager.h"
#include "../Game/Attack/AttackManager.h"
#include "../Game/Actor/Stage/BattleAreaManager.h"
#include "../Game/Actor/Character/Player/Player.h"
#include "../Game/Camera/CameraController.h"
#include "../Game/Camera/PlayerCamera.h"
#include "../Game/Camera/StartCamera.h"
#include "../General/Collision/Physics.h"
#include "../General/CSV/ActorData.h"
#include "../General/CSV/CharaStatusData.h"
#include "../General/AssetManager.h"
#include "../General/Fader.h"
#include "../General/ShaderPostProcess.h"
#include "../General/Timer.h"
#include "../General/SaveDataManager.h"
#include "../Main/Application.h"
#include "../General/Effect/EffekseerManager.h"
#include "../General/Sound/SoundManager.h"
#include "../General/LoadingManager.h"
#include "../Game/UI/UIManager.h"
#include "../Game/UI/Input/InputUI.h"
#include "../Game/Tutorial/TutorialManager.h"

namespace
{
	//CSVデータ名
	const std::wstring kCharacterDataName = L"CharacterData";
	const std::wstring kStageDataName = L"StageData";
	const std::wstring kCheckPointDataName = L"CheckPointData";

	//クリア時のフェードカラー
	constexpr unsigned int kClearFadeColor = 0xeeeeee;

	//BGM
	const std::wstring kBGMStage1Path = L"Stage1";
	const std::wstring kBGMStage1BossPath = L"Stage1Boss";
	const std::wstring kBGMStage2Path = L"Stage2";
	const std::wstring kBGMStage2BossPath = L"Stage2Boss";
	const std::wstring kBGMStage3Path = L"Stage3";
}

GameScene::GameScene(SceneController& controller, std::wstring stageName) :
	SceneBase(controller),
	m_stageName(stageName),
	m_effectManager(EffekseerManager::GetInstance()),
	m_isGameClear(false),
	m_isGameover(false)
{
	
}

GameScene::~GameScene()
{
}

void GameScene::Init()
{
	//アセットを削除
	AssetManager::GetInstance().DeleteModelHandle();
	//アプリケーション
	auto& app = Application::GetInstance();
	//ポストエフェクトを解除
	app.GetPostProcess()->ResetPostEffectState();
	//タイムスケール
	app.SetTimeScale(1.0f);
	//UI削除
	UIManager::GetInstance().AllDeleteUI();
	//Physics
	auto& physics = Physics::GetInstance();
	//リセット
	physics.Reset();
	//Physicsを開始
	physics.StartUpdate();
	//エフェクトリセット
	EffekseerManager::GetInstance().Reset();
	
	//ロード
	auto& loadingManager = LoadingManager::GetInstance();

	//非同期ロード開始
	loadingManager.StartLoading();

	//ステージインデックス
	auto stageName = m_stageName.c_str();

	//カメラ
	auto& cameraController = CameraController::GetInstance();
	cameraController.Init();
	//プレイヤーカメラ
	auto playerCamera = std::make_shared<PlayerCamera>();
	cameraController.ChangeCamera(playerCamera);

	m_attackManager = std::make_shared<AttackManager>();
	m_attackManager->Init();
	m_attackManager->SetPlayerCamera(playerCamera);

	//アクター
	m_actorManager = std::make_shared<ActorManager>();
	m_actorManager->Init();
	m_actorManager->CreateActorCSV(stageName, kCharacterDataName.c_str());
	m_actorManager->CreateActorCSV(stageName, kStageDataName.c_str());
	m_actorManager->CreateActorCSV(stageName, kCheckPointDataName.c_str());

	//音
	InitStageBGM();
	PlayStageBGM();

	//カメラセット
	m_actorManager->SetPlayerCamera(playerCamera);
	//攻撃マネージャーセット
	m_actorManager->SetAttackManager(m_attackManager);

	//エリアマネージャー
	m_battleAreaManager = std::make_shared<BattleAreaManager>();
	m_battleAreaManager->Init(stageName, m_actorManager);

	//エフェクト
	m_effectManager.Init();

	//フェードイン
	Fader::GetInstance().FadeIn();

	//タイマー
	m_timer = std::make_shared<Timer>();
	m_timer->Init();

	//ゲームオーバー
	m_isGameover = false;
	//ゲームクリア
	m_isGameClear = false;

	//スタートカメラ
	auto startCamera = std::make_shared<StartCamera>(m_actorManager->GetPlayer(),m_actorManager);
	cameraController.PushCamera(startCamera);

	//操作UI
	auto inputUI = std::make_shared<InputUI>();
	inputUI->Init();

	//非同期ロード終了
	loadingManager.EndLoading();

}

void GameScene::Update()
{
	//ロード中はスキップ
	if (LoadingManager::GetInstance().IsLoading())return;

	//プレイ時間加算
	SaveDataManager::GetInstance().AddPlayTime();

	//ボス戦に入ったか
	bool isBeforeBossBattle = m_actorManager->IsBossBattle();
	//更新
	m_actorManager->Update();
	m_attackManager->Update();
	CameraController::GetInstance().Update();
	m_battleAreaManager->Update(m_actorManager);
	m_effectManager.Update();
	m_timer->Update();

	//ボス戦に入ったら
	ChangeBossBattleBGM(isBeforeBossBattle);

	auto& input = Input::GetInstance();

	auto& fader = Fader::GetInstance();
	//フェードアウトしたら
	if (fader.IsFinishFadeOut())
	{
		if (m_isGameClear)
		{
			m_controller.ChangeScene(std::make_shared<ResultScene>(m_stageName, m_controller, m_timer));
			return;
		}
		else if (m_isGameover)
		{
			m_controller.PushScene(std::make_shared<GameoverScene>(m_controller));
			return;
		}
	}

	
	if (!fader.IsFadeNow())
	{
		//もしもすべてのエリアを突破したら
		if ((m_battleAreaManager->IsEndAllArea() || input.IsTrigger("GameClear")))
		{
			//ゲームクリア
			m_isGameClear = true;
			//フェードカラー
			fader.SetColor(kClearFadeColor);
			//フェードアウト
			fader.FadeOut();
		}
		//プレイヤーが死亡したら
		else if ((m_actorManager->IsGameover() || input.IsTrigger("Gameover")))
		{
			//ゲームオーバー
			m_isGameover = true;
			//フェードアウト
			fader.FadeOut();
		}
		else if (input.IsTrigger("Pause"))
		{
			if (m_actorManager->GetPlayer().expired())return;
			auto player = m_actorManager->GetPlayer().lock();
			if (player->IsStartState())return;				//スタート状態ならポーズには遷移できない
			if (player->GetCharaStatus()->IsDead())return;	//死亡状態ならポーズには遷移できない
			//ポーズ
			m_controller.PushScene(std::make_shared<PauseScene>(m_controller));
			return;
		}
	}
}


void GameScene::Draw()
{
	//ロード中はスキップ
	if (LoadingManager::GetInstance().IsLoading())return;

	m_actorManager->Draw();
	m_attackManager->Draw();
	m_effectManager.Draw();
	CameraController::GetInstance().Draw();
}

void GameScene::End()
{
	Physics::GetInstance().Reset();
	m_actorManager->End();
	m_attackManager->End();
	m_battleAreaManager->End();
	m_effectManager.End();
}

void GameScene::Restart()
{
	//ゲームオーバー
	m_isGameover = false;

	//ゲームクリア
	m_isGameClear = false;

	//リセット
	Physics::GetInstance().Reset();
	//Physicsを開始
	Physics::GetInstance().StartUpdate();

	//エフェクト
	m_effectManager.End();
	m_effectManager.Init();

	//ポストエフェクトを解除
	Application::GetInstance().GetPostProcess()->ResetPostEffectState();

	//タイムスケール
	Application::GetInstance().SetTimeScale(1.0f);

	//UI削除
	UIManager::GetInstance().AllDeleteUI();

	//Inputの入力情報リセット
	Input::GetInstance().StopUpdate();


	//カメラ
	auto camera = std::make_shared<PlayerCamera>();
	auto& cameraController = CameraController::GetInstance();
	cameraController.Init();
	cameraController.ChangeCamera(camera);

	//アクターの再スタート
	m_actorManager->Restart();
	m_actorManager->SetPlayerCamera(camera);

	//攻撃マネージャー
	m_attackManager->Init();
	m_attackManager->SetPlayerCamera(camera);

	//エリアの再スタート
	m_battleAreaManager->Restart();

	//音
	PlayStageBGM();

	//フェードイン
	Fader::GetInstance().FadeIn();
}

void GameScene::DebugDraw() const
{
#if _DEBUG
	DrawString(0, 0, L"Game Scene", 0xffffff);
	DrawString(0, 16, L"[D]キーで Debug Scene", 0xffffff);
#endif
}

void GameScene::PlayStageBGM()
{
	std::wstring bgmPath;
	switch (GetStageIndexByName(m_stageName))
	{
	case StageIndex::Stage1:
		if (m_actorManager->IsBossBattle())
		{
			bgmPath = kBGMStage1BossPath;
		}
		else
		{
			bgmPath = kBGMStage1Path;
		}
		break;
	case StageIndex::Stage2:
		if (m_actorManager->IsBossBattle())
		{
			bgmPath = kBGMStage2BossPath;
		}
		else
		{
			bgmPath = kBGMStage2Path;
		}
		break;
	case StageIndex::Stage3:
		bgmPath = kBGMStage3Path;
		break;
	default:
		bgmPath = kBGMStage1Path;
		break;
	}
	SoundManager::GetInstance().PlayBGM(bgmPath);
}

void GameScene::InitStageBGM()
{
	//音リセット
	auto& soundManager = SoundManager::GetInstance();
	soundManager.Reset();

	//ステージBGMで使うサウンドデータを先に全てロードする
	switch (GetStageIndexByName(m_stageName))
	{
	case StageIndex::Stage1:
		soundManager.LoadBGM(kBGMStage1BossPath);
		soundManager.LoadBGM(kBGMStage1Path);
		break;
	case StageIndex::Stage2:
		soundManager.LoadBGM(kBGMStage2BossPath);
		soundManager.LoadBGM(kBGMStage2Path);
		break;
	case StageIndex::Stage3:
		soundManager.LoadBGM(kBGMStage3Path);
		break;
	default:
		break;
	}
	
}


void GameScene::ChangeBossBattleBGM(bool isBeforeBossBattle)
{
	if (isBeforeBossBattle != m_actorManager->IsBossBattle())
	{
		//BGM変更
		PlayStageBGM();
	}
}