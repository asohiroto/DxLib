#include "GameoverScene.h"
#include "SceneController.h"
#include "SelectScene.h"
#include "../General/AssetManager.h"
#include "../General/Fader.h"
#include "../General/Input.h"
#include "../General/SaveDataManager.h"
#include "../General/Collision/Physics.h"
#include "../General/Sound/SoundManager.h"
#include "../Game/UI/Gameover/GameoverUI.h"
#include "../Game/UI/UIManager.h"

namespace
{
	constexpr unsigned int kFadeColor = 0x000000;

	//BGM
	const std::wstring kBGMGameoverPath = L"GameoverScene";
	//SE
	const std::wstring kSESelectPath = L"Select";
	const std::wstring kSEOKPath = L"OK";
}

GameoverScene::GameoverScene(SceneController& controller):
	SceneBase(controller),
	m_menuIndex(Menu::Continue)
{
}

GameoverScene::~GameoverScene()
{
}

void GameoverScene::Init()
{
	//フェードイン
	Fader::GetInstance().FadeIn();

	//UIの削除
	UIManager::GetInstance().AllDeleteUI();
	//Phisicsを停止
	Physics::GetInstance().StopUpdate();

	//サウンド
	auto& soundManager = SoundManager::GetInstance();
	soundManager.Reset();
	soundManager.LoadBGM(kBGMGameoverPath);
	soundManager.LoadSE(kSEOKPath);
	soundManager.LoadSE(kSESelectPath);
	//BGM
	soundManager.PlayBGM(kBGMGameoverPath);

	auto gameoverUI = std::make_shared<GameoverUI>();
	gameoverUI->Init();
	m_pGameoverUI = gameoverUI;
}

void GameoverScene::Update()
{
	//プレイ時間加算
	SaveDataManager::GetInstance().AddPlayTime();

	auto& input = Input::GetInstance();
	auto& fader = Fader::GetInstance();
	auto& soundManager = SoundManager::GetInstance();
	//フェードアウトしきったら
	if (fader.IsFinishFadeOut())
	{
		if (m_controller.GetBaseScene().expired())return;
		auto baseScene = m_controller.GetBaseScene().lock();
		switch (m_menuIndex)
		{
		case GameoverScene::Menu::Continue:
			baseScene->Restart();
			break;
		case GameoverScene::Menu::Restart:
			baseScene->End();
			baseScene->Init();
			break;
		case GameoverScene::Menu::StageSelect:
			m_controller.ChangeBaseScene(std::make_unique<SelectScene>(m_controller));
			break;
		default:
			m_controller.ChangeBaseScene(std::make_unique<SelectScene>(m_controller));
			break;
		}
		m_controller.PopScene();
		return;
	}
	//ボタンを押したら次
	if (m_pGameoverUI.expired())return;
	auto ui = m_pGameoverUI.lock();
	if (!fader.IsFadeNow() && ui->IsFinish())
	{
		if (input.IsTrigger("A"))
		{
			//SE再生
			soundManager.PlayOnceSE(kSEOKPath);
			//フェードカラー
			fader.SetColor(kFadeColor);
			//フェードアウト
			fader.FadeOut();
		}
		//カーソル
		int index = static_cast<int>(m_menuIndex);
		if (input.IsTrigger("Up"))--index;
		if (input.IsTrigger("Down"))++index;
		index = MathSub::ClampInt(index, static_cast<int>(Menu::Continue), static_cast<int>(Menu::StageSelect));
		//カーソルを動かしたら
		if (index != static_cast<int>(m_menuIndex))
		{
			//SE再生
			soundManager.PlayOnceSE(kSESelectPath);
		}
		m_menuIndex = static_cast<Menu>(index);
		ui->SetMenuIndex(m_menuIndex);
	}

}

void GameoverScene::Draw()
{
}

void GameoverScene::End()
{
	//Phisicsを開始
	Physics::GetInstance().StartUpdate();
}

void GameoverScene::DebugDraw() const
{
}
