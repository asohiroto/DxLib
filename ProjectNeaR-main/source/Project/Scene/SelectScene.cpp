#include "SelectScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "OptionScene.h"
#include "TutorialScene.h"
#include "../General/Input.h"
#include <DxLib.h>
#include "../General/StringUtil.h"
#include "../General/Math/MyMath.h"
#include "../General/AssetManager.h"
#include "../General/ShaderPostProcess.h"
#include "../General/Effect/EffekseerManager.h"
#include "../General/Timer.h"
#include "../Game/UI/UIManager.h"
#include "../Game/UI/TalkUI.h"
#include "../Main/Application.h"
#include "../General/CSV/CSVDataLoader.h"
#include "../General/CSV/TextData.h"
#include "../General/Fader.h"
#include "../General/Sound/SoundManager.h"
#include "../General/SaveDataManager.h"
#include "../General/CSV/ClearSaveData.h"
#include "../General/Collision/Physics.h"
#include "../Game/Actor/ActorManager.h"
#include "../Game/UI/Select/SelectUI.h"
#include "../Game/Camera/SelectCamera.h"
#include "../Game/Camera/CameraController.h"

namespace
{
	//チュートリアル
	const Vector3 kTutorialViewPos = { -5.0f, 10.0f, -50.0f };
	const Vector3 kTutorialPos = { 10.0f, 10.0f, 10.0f };
	//ステージ
	const Vector3 kStageViewPos = { -50.0f, 10.0f, -50.0f };
	const Vector3 kStagePos = { 0.0f, 20.0f, 10.0f };
	//オプション
	const Vector3 kOptionViewPos = { 50.0f, 10.0f, -20.0f };
	const Vector3 kOptionPos = { 0.0f, 20.0f, 0.0f };
	//オプション
	const Vector3 kTitleViewPos = { 0.0f, 10.0f, 30.0f };
	const Vector3 kTitlePos = { 0.0f, 10.0f, 0.0f };

	//パス
	const std::wstring kSelect = L"Select";
	const std::wstring kCharacterDataPath = L"CharacterData";
	const std::wstring kStageDataPath = L"StageData";
	
	//BGM
	const std::wstring kBGMSelectPath = L"SelectScene";
	//SE
	const std::wstring kSESelectPath = L"Select";
	const std::wstring kSEOKPath = L"OK";
	const std::wstring kSECancelPath = L"Cancel";
}

SelectScene::SelectScene(SceneController& controller) :
	SceneBase(controller),
	m_currentMainMenu(MainMenu::Tutorial),
	m_currentStageMenu(StageMenu::Stage1),
	m_canSelectStageMenu(StageMenu::Stage1),
	m_currentTutorialMenu(TutorialMenu::Tutorial1),
	m_update(&SelectScene::UpdateMainMenu)
{
}

SelectScene::~SelectScene()
{
}

void SelectScene::Init()
{
	//アセットを削除
	AssetManager::GetInstance().AllDelete();
	//リセット
	Physics::GetInstance().Reset();
	//ポストエフェクトを解除
	Application::GetInstance().GetPostProcess()->ResetPostEffectState();
	//UI削除
	UIManager::GetInstance().AllDeleteUI();
	//エフェクトリセット
	EffekseerManager::GetInstance().Reset();
	//音リセット
	SoundManager::GetInstance().Reset();
	//フェードイン
	Fader::GetInstance().FadeIn();

	//一番上の項目から
	m_currentMainMenu = MainMenu::Tutorial;
	m_currentStageMenu = StageMenu::Stage1;
	m_currentTutorialMenu = TutorialMenu::Tutorial1;

	//クリア状況から選べるステージを制限
	auto clearData = SaveDataManager::GetInstance().GetClearData();
	//ステージ2,3までクリアしていたら
	if (clearData->IsClearStage2() || clearData->IsClearStage3())
	{
		//ステージ3まで解放
		m_canSelectStageMenu = StageMenu::Stage3;
	}
	//ステージ1までクリアしているなら
	else if (clearData->IsClearStage1())
	{
		//ステージ2まで解放
		m_canSelectStageMenu = StageMenu::Stage2;
	}
	//一つもクリアしていないなら
	else if (!clearData->IsClearStage1())
	{
		//ステージ1まで解放
		m_canSelectStageMenu = StageMenu::Stage1;
	}

	//UI作成
	auto ui = std::make_shared<SelectUI>();
	ui->Init();
	m_selectStageUI = ui;

	//音
	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadBGM(kBGMSelectPath);
	soundManager.LoadSE(kSESelectPath);
	soundManager.LoadSE(kSEOKPath);
	soundManager.LoadSE(kSECancelPath);
	//BGM
	soundManager.PlayBGM(kBGMSelectPath);

	//カメラ
	auto camera = std::make_shared<SelectCamera>();
	auto& cameraController = CameraController::GetInstance();
	cameraController.Init();
	cameraController.ChangeCamera(camera);
	m_selectCamera = camera;

	//アクターマネージャー
	m_actorManager = std::make_shared<ActorManager>();
	m_actorManager->Init();
	m_actorManager->CreateActorCSV(kSelect.c_str(), kCharacterDataPath.c_str());
	m_actorManager->CreateActorCSV(kSelect.c_str(), kStageDataPath.c_str());

	//セーブ
	SaveDataManager::GetInstance().SaveAllData();
}

void SelectScene::Update()
{
	auto& input = Input::GetInstance();
	auto& fader = Fader::GetInstance();
	//更新
	auto& cameraController = CameraController::GetInstance();
	cameraController.Update();
	m_actorManager->Update();

	//プレイ時間加算
	SaveDataManager::GetInstance().AddPlayTime();

	if (m_selectCamera.expired())return;
	
	//状態ごとの更新処理
	(this->*m_update)(input, fader, m_selectCamera.lock());
}

void SelectScene::Draw()
{
	DrawSphere3D(VGet(0.0f, 0.0f, 300.0f), 30.0f, 16, 0xff0000, 0xff0000, true);
	m_actorManager->Draw();
}

void SelectScene::End()
{
	m_actorManager->End();
}

void SelectScene::DebugDraw() const
{
}

void SelectScene::UpdateMainMenu(Input& input, Fader& fader,std::shared_ptr<SelectCamera> startCamera)
{
	//フェードアウトしきったら
	if (fader.IsFinishFadeOut())
	{
		m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
		return;
	}

	//サウンド
	auto& soundManager = SoundManager::GetInstance();

	//引きカメラ
	startCamera->PullCamera();

	//フェード中は操作できない
	if (!fader.IsFadeNow())
	{
		if (input.IsTrigger("A"))
		{
			//SE再生
			soundManager.PlayOnceSE(kSEOKPath);

			switch (m_currentMainMenu)
			{
			case SelectScene::MainMenu::Tutorial:
				//チュートリアルセレクトへ遷移
				m_update = &SelectScene::UpdateTutorialMenu;
				break;
			case SelectScene::MainMenu::Stage:
				//ステージセレクトへ遷移
				m_update = &SelectScene::UpdateStageMenu;
				break;
			case SelectScene::MainMenu::Option:
				//オプション
				m_controller.PushScene(std::make_shared<OptionScene>(m_controller));
				break;
			case SelectScene::MainMenu::Title:
				//タイトルへ
				fader.FadeOut();
				break;
			default:
				break;
			}
			return;
		}

		int menu = static_cast<int>(m_currentMainMenu);
		if (input.IsRepeate("Up"))menu--;
		if (input.IsRepeate("Down"))menu++;

		//カーソルが動いたら
		if (menu != static_cast<int>(m_currentMainMenu))
		{
			//SE再生
			soundManager.PlayOnceSE(kSESelectPath);
		}

		//範囲外ならループ
		if (menu < static_cast<int>(MainMenu::Tutorial))
		{
			menu = static_cast<int>(MainMenu::Title);
		}
		if (menu > static_cast<int>(MainMenu::Title))
		{
			menu = static_cast<int>(MainMenu::Tutorial);
		}
		m_currentMainMenu = static_cast<MainMenu>(menu);

		//UI
		if (!m_selectStageUI.expired())
		{
			m_selectStageUI.lock()->SetSelectMainMenuIndex(menu);
		}

		//カメらの視点を変更
		switch (m_currentMainMenu)
		{
		case MainMenu::Tutorial:
			startCamera->SetViewPos(kTutorialViewPos);
			startCamera->SetPos(kTutorialPos);
			break;
		case MainMenu::Stage:
			startCamera->SetViewPos(kStageViewPos);
			startCamera->SetPos(kStagePos);
			break;
		case MainMenu::Option:
			startCamera->SetViewPos(kOptionViewPos);
			startCamera->SetPos(kOptionPos);
			break;
		case MainMenu::Title:
			startCamera->SetViewPos(kTitleViewPos);
			startCamera->SetPos(kTitlePos);
			break;
		}
	}
}

void SelectScene::UpdateTutorialMenu(Input& input, Fader& fader, std::shared_ptr<SelectCamera> startCamera)
{
	//ズームカメラ
	startCamera->ZoomInCamera();

	//サウンド
	auto& soundManager = SoundManager::GetInstance();

	//フェードアウトしきったら
	if (fader.IsFinishFadeOut())
	{
		switch (m_currentTutorialMenu)
		{
		case SelectScene::TutorialMenu::Tutorial1:
			m_controller.ChangeScene(std::make_shared<TutorialScene>(m_controller, GetStageNameByIndex(StageIndex::Tutorial1)));
			break;
		case SelectScene::TutorialMenu::Tutorial2:
			m_controller.ChangeScene(std::make_shared<TutorialScene>(m_controller, GetStageNameByIndex(StageIndex::Tutorial2)));
			break;
		case SelectScene::TutorialMenu::Tutorial3:
			m_controller.ChangeScene(std::make_shared<TutorialScene>(m_controller, GetStageNameByIndex(StageIndex::Tutorial3)));
			break;
		default:
			break;
		}
		return;
	}

	//フェード中は操作できない
	if (!fader.IsFadeNow())
	{
		if(input.IsTrigger("A"))
		{
			//SE再生
			soundManager.PlayOnceSE(kSEOKPath);
			//フェード
			fader.FadeOut();
			return;
		}
		if(input.IsTrigger("B"))
		{
			//SE再生
			soundManager.PlayOnceSE(kSECancelPath);
			//メインメニューへ戻る
			m_update = &SelectScene::UpdateMainMenu;
			return;
		}

		int menu = static_cast<int>(m_currentTutorialMenu);
		if (input.IsRepeate("Up"))menu--;
		if (input.IsRepeate("Down"))menu++;

		//カーソルが動いたら
		if (menu != static_cast<int>(m_currentTutorialMenu))
		{
			//SE再生
			soundManager.PlayOnceSE(kSESelectPath);
		}

		//範囲外ならループ
		if (menu < static_cast<int>(TutorialMenu::Tutorial1))
		{
			menu = static_cast<int>(TutorialMenu::Tutorial3);
		}
		if (menu > static_cast<int>(TutorialMenu::Tutorial3))
		{
			menu = static_cast<int>(TutorialMenu::Tutorial1);
		}

		m_currentTutorialMenu = static_cast<TutorialMenu>(menu);

		//UI
		if (!m_selectStageUI.expired())
		{
			m_selectStageUI.lock()->SetSelectTutorialMenuIndex(menu);
		}
	}
}

void SelectScene::UpdateStageMenu(Input& input, Fader& fader, std::shared_ptr<SelectCamera> startCamera)
{
	//ズームカメラ
	startCamera->ZoomInCamera();

	//サウンド
	auto& soundManager = SoundManager::GetInstance();

	//フェードアウトしきったら
	if (fader.IsFinishFadeOut())
	{
		switch (m_currentStageMenu)
		{
		case SelectScene::StageMenu::Stage1:
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller,GetStageNameByIndex(StageIndex::Stage1)));
			break;
		case SelectScene::StageMenu::Stage2:
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller,GetStageNameByIndex(StageIndex::Stage2)));
			break;
		case SelectScene::StageMenu::Stage3:
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller,GetStageNameByIndex(StageIndex::Stage3)));
			break;
		default:
			break;
		}
		return;
	}

	//フェード中は操作できない
	if (!fader.IsFadeNow())
	{
		if (input.IsTrigger("A"))
		{
			//SE再生
			soundManager.PlayOnceSE(kSEOKPath);
			//フェード
			fader.FadeOut();
			return;
		}
		if (input.IsTrigger("B"))
		{
			//SE再生
			soundManager.PlayOnceSE(kSECancelPath);
			//メインメニューへ戻る
			m_update = &SelectScene::UpdateMainMenu;
			return;
		}

		int menu = static_cast<int>(m_currentStageMenu);
		if (input.IsRepeate("Up"))menu--;
		if (input.IsRepeate("Down"))menu++;

		//カーソルが動いたら
		if (menu != static_cast<int>(m_currentStageMenu))
		{
			//SE再生
			soundManager.PlayOnceSE(kSESelectPath);
		}

		//範囲外ならループ
		if (menu < static_cast<int>(StageMenu::Stage1))
		{
			menu = static_cast<int>(m_canSelectStageMenu);
		}
		if (menu > static_cast<int>(m_canSelectStageMenu))
		{
			menu = static_cast<int>(StageMenu::Stage1);
		}

		m_currentStageMenu = static_cast<StageMenu>(menu);

		//UI
		if (!m_selectStageUI.expired())
		{
			m_selectStageUI.lock()->SetSelectStageMenuIndex(menu);
		}
	}
}
