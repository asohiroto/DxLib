#include "TitleScene.h"
#include <Dxlib.h>
#include  "../General/Input.h"
#include "SceneController.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "OptionScene.h"
#include "DebugScene/DebugSelectScene.h"
#include "../General/Fader.h"
#include "../General/Game.h"
#include "../General/Collision/Physics.h"
#include "../Game/UI/Title/TitleUI.h"
#include "../Game/UI/Title/SelectTitleUI.h"
#include "../Game/UI/UIManager.h"
#include "../Game/UI/Dialog/DialogUI.h"
#include "../Game/Actor/ActorManager.h"
#include "../General/AssetManager.h"
#include "../General/SaveDataManager.h"
#include "../Game/Camera/CameraController.h"
#include "../Game/Camera/TitleCamera.h"
#include "../Main/Application.h"
#include "../General/ShaderPostProcess.h"
#include "../General/Math/MyMath.h"
#include "../General/Effect/EffekseerManager.h"
#include "../General/Sound/SoundManager.h"
#include "../General/Sound/SE.h"
#include "../General/LoadingManager.h"

namespace
{
	//最初のグリッジフレーム
	constexpr int kStartHardShakingFrame = 100;
	//強いグリッジ
	constexpr float kBlockScele = 10.0f;
	constexpr float kNoiseSpeed = 10.0f;
	constexpr float kShakeStrength = 10.0f;

	//タイトル
	const std::wstring kTitlePath = L"Title";
	const std::wstring kCharacterDataPath = L"CharacterData";
	const std::wstring kStageDataPath = L"StageData";

	//BGM
	const std::wstring kBGMTitlePath = L"TitleScene";
	//SE
	const std::wstring kSENoisePath = L"Noise";
	const std::wstring kSESelectPath = L"Select";
	const std::wstring kSEOKPath = L"OK";
	const std::wstring kSECancelPath = L"Cancel";
	const std::wstring kSEGameStartPath = L"GameStart";

	//ダイアログテキスト
	const std::wstring kDialogText = L"現在のセーブデータを破棄して\n新しくデータを作成しますか?";
}

TitleScene::TitleScene(SceneController& controller):
	SceneBase(controller),
	m_hardShakingCountFrame(kStartHardShakingFrame),
	m_blockScele(kBlockScele),
	m_noiseSpeed(kNoiseSpeed),
	m_shakeStrength(kShakeStrength),
	m_effectManager(EffekseerManager::GetInstance()),
	m_noiseSE(),
	m_selectIndex(SelectMenuTitle::Continue),
	m_update(&TitleScene::UpdateTitle)
{
}

TitleScene::~TitleScene()
{
	if (!m_selectTitleUI.expired())
	{
		m_selectTitleUI.lock()->Delete();
	}
	if (!m_dialogUI.expired())
	{
		m_dialogUI.lock()->Delete();
	}
	if (!m_noiseSE.expired())
	{
		m_noiseSE.lock()->Delete();
	}
}

void TitleScene::Init()
{
	//アセットを削除
	AssetManager::GetInstance().AllDelete();
	
	//リセット
	Physics::GetInstance().Reset();

	//ポストエフェクトを解除
	Application::GetInstance().GetPostProcess()->ResetPostEffectState();

	//UI削除
	UIManager::GetInstance().AllDeleteUI();

	//音リセット
	SoundManager::GetInstance().Reset();

	//エフェクトリセット
	m_effectManager.Reset();

	//ポストエフェクトリセット
	auto& app = Application::GetInstance();
	app.GetPostProcess()->ResetPostEffectState();

	//タイムスケール
	app.SetTimeScale(1.0f);

	//フェードイン
	Fader::GetInstance().FadeIn();

	//エフェクトマネージャー初期化
	m_effectManager.Init();

	//ロード
	auto& loadingManager = LoadingManager::GetInstance();

	//音
	auto& soundManager = SoundManager::GetInstance();

	//非同期ロード開始
	loadingManager.StartLoading();

	//アクターマネージャー
	m_actorManager = std::make_shared<ActorManager>();
	m_actorManager->Init();
	m_actorManager->CreateActorCSV(kTitlePath.c_str(), kCharacterDataPath.c_str());
	m_actorManager->CreateActorCSV(kTitlePath.c_str(), kStageDataPath.c_str());


	//サウンドデータのロード(非同期で読み込もうとすると失敗してしまうので同期で)
	soundManager.LoadBGM(kBGMTitlePath);
	soundManager.LoadSE(kSENoisePath);
	soundManager.LoadSE(kSEGameStartPath);
	soundManager.LoadSE(kSEOKPath);

	//カメラ
	auto camera = std::make_shared<TitleCamera>();
	auto& cameraController = CameraController::GetInstance();
	cameraController.Init();
	cameraController.ChangeCamera(camera);

	//タイトルロゴ
	auto titleLogo = std::make_shared<TitleUI>();
	titleLogo->Init();

	//セレクトUI
	auto selectTitleUI = std::make_shared<SelectTitleUI>();
	selectTitleUI->Init();
	selectTitleUI->DisableDraw();
	m_selectTitleUI = selectTitleUI;

	//ダイアログUI
	auto dialogUI = std::make_shared<DialogUI>(kDialogText);
	dialogUI->Init();
	dialogUI->DisableDraw();
	m_dialogUI = dialogUI;

	//非同期ロード終了
	loadingManager.EndLoading();

	//BGM再生
	soundManager.PlayBGM(kBGMTitlePath);

	//ノイズ再生
	m_noiseSE = soundManager.PlayLoopSE(kSENoisePath);

	//グリッジ
	auto& postPrecess = Application::GetInstance().GetPostProcess();
	postPrecess->AddPostEffectState(ShaderPostProcess::PostEffectState::NoColorGlitch);
	postPrecess->SetBlockScele(m_blockScele);
	postPrecess->SetNoiseSpeed(m_noiseSpeed);
	postPrecess->SetShakeStrength(m_shakeStrength);

	//番号初期化
	m_selectIndex = SelectMenuTitle::Continue;

	//更新
	m_update = &TitleScene::UpdateTitle;
}

void TitleScene::Update()
{
	//ロード中はスキップ
	if (LoadingManager::GetInstance().IsLoading())return;

	auto& input = Input::GetInstance();

	auto& fader = Fader::GetInstance();

	//カメラ
	auto& cameraController = CameraController::GetInstance();
	cameraController.Update();

	//エフェクトの再生
	m_effectManager.Update();

	//アクター更新
	m_actorManager->Update();

	//グリッジの更新
	UpdateGlitch();

	//状態ごとの更新処理
	(this->*m_update)(input, fader);
}

void TitleScene::Draw()
{
	//ロード中はスキップ
	if (LoadingManager::GetInstance().IsLoading())return;

	m_actorManager->Draw();
	m_effectManager.Draw();
}

void TitleScene::End()
{
	//グリッジ削除
	auto& postPrecess = Application::GetInstance().GetPostProcess();
	postPrecess->SubPostEffectState(ShaderPostProcess::PostEffectState::NoColorGlitch);

	m_actorManager->End();
}

void TitleScene::DebugDraw() const
{
#if _DEBUG
	DrawString(0, 0, L"Title Scene", 0xffffff);
	DrawString(0, 16, L"[D]キーで Debug Scene", 0xffffff);
#endif
}


void TitleScene::UpdateTitle(Input& input, Fader& fader)
{
	if (input.IsTrigger("A") && !fader.IsFadeNow())
	{
		//クリックSE
		auto& soundManager = SoundManager::GetInstance();
		soundManager.PlayOnceSE(kSEOKPath);

		//セレクトUI表示
		if (!m_selectTitleUI.expired())
		{
			m_selectTitleUI.lock()->EnableDraw();
		}

		//セレクト更新
		m_update = &TitleScene::UpdateSelect;
		return;
	}
}

void TitleScene::UpdateSelect(Input& input, Fader& fader)
{
	//フェードアウトしきったら
	if (fader.IsFinishFadeOut())
	{
		switch (m_selectIndex)
		{
		case TitleScene::SelectMenuTitle::Continue:
			m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
			break;
		case TitleScene::SelectMenuTitle::NewGame:
			//データを削除してから始める
			SaveDataManager::GetInstance().SaveNewData();
			m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
			break;
		case TitleScene::SelectMenuTitle::GameEnd:
			Application::GetInstance().FinishApplication();
			break;
		default:
			break;
		}
		return;
	}

	if (!fader.IsFadeNow())
	{
		//セレクトUI
		if (m_selectTitleUI.expired())return;
		auto selectUI = m_selectTitleUI.lock();

		auto& soundManager = SoundManager::GetInstance();
		if (input.IsTrigger("B"))
		{
			//非表示
			selectUI->DisableDraw();

			//キャンセルSE
			soundManager.PlayOnceSE(kSECancelPath);

			//タイトル更新
			m_update = &TitleScene::UpdateTitle;
			return;
		}
		if (input.IsTrigger("A"))
		{
			//オプション
			if (m_selectIndex == SelectMenuTitle::Option)
			{
				//クリックSE	
				soundManager.PlayOnceSE(kSEOKPath);
				m_controller.PushScene(std::make_shared<OptionScene>(m_controller));
				return;
			}
			//初めから
			if (m_selectIndex == SelectMenuTitle::NewGame)
			{
				//クリックSE	
				soundManager.PlayOnceSE(kSEOKPath);
				
				//ダイアログ更新
				m_update = &TitleScene::UpdateDialog;
				return;
			}

			//フェード
			fader.FadeOut();

			//スタートSE	
			soundManager.PlayOnceSE(kSEGameStartPath);
			return;
		}

		//メニューセレクト
		int index = static_cast<int>(m_selectIndex);
		if (input.IsRepeate("Up"))--index;
		if (input.IsRepeate("Down"))++index;
		if (index < static_cast<int>(SelectMenuTitle::Continue))index = static_cast<int>(SelectMenuTitle::GameEnd);
		if (index > static_cast<int>(SelectMenuTitle::GameEnd))index = static_cast<int>(SelectMenuTitle::Continue);

		//セレクトSE
		if (index != static_cast<int>(m_selectIndex))soundManager.PlayOnceSE(kSESelectPath);

		m_selectIndex = static_cast<SelectMenuTitle>(index);

		//UI反映
		selectUI->SetMenuIndex(m_selectIndex);
	}
}

void TitleScene::UpdateDialog(Input& input, Fader& fader)
{
	//フェードアウトしきったら
	if (fader.IsFinishFadeOut())
	{
		switch (m_selectIndex)
		{
		case TitleScene::SelectMenuTitle::Continue:
			m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
			break;
		case TitleScene::SelectMenuTitle::NewGame:
			//データを削除してから始める
			SaveDataManager::GetInstance().SaveNewData();
			m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
			break;
		case TitleScene::SelectMenuTitle::GameEnd:
			Application::GetInstance().FinishApplication();
			break;
		default:
			break;
		}
		return;
	}

	if (m_dialogUI.expired())return;
	auto dialogUI = m_dialogUI.lock();
	//表示
	dialogUI->EnableDraw();

	if (!fader.IsFadeNow())
	{
		auto& soundManager = SoundManager::GetInstance();
		if (input.IsTrigger("B"))
		{
			//キャンセルSE
			soundManager.PlayOnceSE(kSECancelPath);

			//非表示
			dialogUI->DisableDraw();

			//セレクト更新
			m_update = &TitleScene::UpdateSelect;
			return;
		}

		//決定
		if (input.IsTrigger("A"))
		{

			if (!dialogUI->IsYes())
			{
				//キャンセルSE
				soundManager.PlayOnceSE(kSECancelPath);

				//非表示
				dialogUI->DisableDraw();

				//セレクト更新
				m_update = &TitleScene::UpdateSelect;
				return;
			}
			//フェード
			fader.FadeOut();

			//スタートSE
			soundManager.PlayOnceSE(kSEGameStartPath);
			return;
		}

		//「はい」か「いいえ」か
		if (input.IsTrigger("Left"))dialogUI->Yes();
		if (input.IsTrigger("Right"))dialogUI->No();
	}
}

void TitleScene::UpdateGlitch()
{
	auto& postPrecess = Application::GetInstance().GetPostProcess();

	//最初の数フレームは激しくグリッジ
	if (m_hardShakingCountFrame > 0)
	{
		//激しく
		--m_hardShakingCountFrame;
		//グリッジの各値の設定
		postPrecess->SetBlockScele(m_blockScele);
		postPrecess->SetNoiseSpeed(m_noiseSpeed);
		postPrecess->SetShakeStrength(m_shakeStrength);
	}
	else
	{
		//ノイズSE停止
		if (!m_noiseSE.expired())
		{
			m_noiseSE.lock()->Delete();
		}
		//ノイズ終了
		postPrecess->SubPostEffectState(ShaderPostProcess::PostEffectState::NoColorGlitch);
	}
}
