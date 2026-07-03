#include "TutorialResultScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "TutorialScene.h"
#include "SelectScene.h"
#include <DxLib.h>
#include "../General/Fader.h"
#include "../General/Input.h"
#include "../General/Game.h"
#include "../General/Sound/SoundManager.h"
#include "../General/Collision/Physics.h"
#include "../General/SaveDataManager.h"
#include "../General/CSV/ClearSaveData.h"
#include "../Game/UI/Result/TutorialResultUI.h"
#include "../Game/UI/UIManager.h"
#include "../Game/Camera/CameraController.h"

namespace
{
	//BGM
	const std::wstring kBGMTutorialResultPath = L"ResultScene";
	//SE
	const std::wstring kSESelectPath = L"Select";
	const std::wstring kSEOKPath = L"OK";
}

TutorialResultScene::TutorialResultScene(SceneController& controller, StageIndex index):
	SceneBase(controller),
	m_tutorialIndex(index),
	m_menuIndex(Menu::Next)
{

}

TutorialResultScene::~TutorialResultScene()
{
}

void TutorialResultScene::Init()
{
	//UI削除
	UIManager::GetInstance().AllDeleteUI();
	//Phisicsを停止
	Physics::GetInstance().StopUpdate();
	auto ui = std::make_shared<TutorialResultUI>(m_tutorialIndex);
	ui->Init();
	m_mcUI = ui;

	//サウンド
	auto& soundManager = SoundManager::GetInstance();
	soundManager.Reset();
	soundManager.LoadBGM(kBGMTutorialResultPath);
	soundManager.LoadSE(kSESelectPath);
	soundManager.LoadSE(kSEOKPath);
	//BGM
	soundManager.PlayBGM(kBGMTutorialResultPath);

	//ステージクリア
	auto clearData = SaveDataManager::GetInstance().GetClearData();
	switch (m_tutorialIndex)
	{
	case StageIndex::Tutorial1:
		clearData->ClearTutorial1();
		break;
	case StageIndex::Tutorial2:
		clearData->ClearTutorial2();
		break;
	case StageIndex::Tutorial3:
		clearData->ClearTutorial3();
		break;
	default:
		break;
	}
}

void TutorialResultScene::Update()
{
	//プレイ時間加算
	SaveDataManager::GetInstance().AddPlayTime();

	auto& input = Input::GetInstance();
	auto& fader = Fader::GetInstance();

	//カメラ
	CameraController::GetInstance().Update();
	//サウンド
	auto& soundManager = SoundManager::GetInstance();

	//フェードアウトしきったら
	if (fader.IsFinishFadeOut())
	{
		if (m_tutorialIndex != StageIndex::Tutorial3)
		{
			//次のチュートリアルへ
			int index = static_cast<int>(m_tutorialIndex) + 1;
			m_tutorialIndex = static_cast<StageIndex>(index);
		}
		else
		{
			//セレクトシーンへ
			m_menuIndex = Menu::Select;
		}

		switch (m_menuIndex)
		{
		case TutorialResultScene::Menu::Next:
			m_controller.ChangeBaseScene(std::make_shared<TutorialScene>(m_controller,GetStageNameByIndex(m_tutorialIndex)));
			break;
		case TutorialResultScene::Menu::Select:
			m_controller.ChangeBaseScene(std::make_shared<SelectScene>(m_controller));
			break;
		default:
			m_controller.ChangeBaseScene(std::make_shared<SelectScene>(m_controller));
			break;
		}
		m_controller.PopScene();
		return;
	}
	//ボタンを押したら次
	if (m_mcUI.expired())return;
	auto ui = m_mcUI.lock();
	if (!fader.IsFadeNow() && ui->IsFinish())
	{
		if (input.IsTrigger("A"))
		{
			//SE再生
			soundManager.PlayOnceSE(kSEOKPath);
			//フェード
			fader.FadeOut();
		}
		//カーソル
		int index = static_cast<int>(m_menuIndex);
		if (input.IsTrigger("Up"))--index;
		if (input.IsTrigger("Down"))++index;
		index = MathSub::ClampInt(index, static_cast<int>(Menu::Next), static_cast<int>(Menu::Select));
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

void TutorialResultScene::Draw()
{
}

void TutorialResultScene::End()
{
	//Phisicsを開始
	Physics::GetInstance().StartUpdate();
}

void TutorialResultScene::DebugDraw() const
{
#if _DEBUG
	DrawString(0, 0, L"TutorialResult Scene", 0xffffff);
	DrawString(0, 16, L"[D]キーで Debug Scene", 0xffffff);
#endif
}
