#include "OptionScene.h"
#include "PauseScene.h"
#include "SceneController.h"
#include "../Game/UI/Option/OptionUI.h"
#include "../Game/UI/UIManager.h"
#include "../Game/Camera/CameraController.h"
#include "../General/Input.h"
#include "../General/Sound/SoundManager.h"
#include "../General/SaveDataManager.h"
#include "../Main/Application.h"

OptionScene::OptionScene(SceneController& controller):
	SceneBase(controller),
	m_optionUI(),
	m_currentOptionMenu(OptionMenu::BGM),
	m_isWindowScreen(false),
	m_update(&OptionScene::UpdateMenuSelect)
{

}

OptionScene::~OptionScene()
{
	//保存
	SaveDataManager::GetInstance().SaveOptionData();

	if (!m_optionUI.expired())
	{
		m_optionUI.lock()->Delete();
	}

	//ポーズシーンがシーンの一番上にあるならUI描画開始
	if (m_controller.GetTopScene().expired())return;
	if (!std::dynamic_pointer_cast<PauseScene>(m_controller.GetTopScene().lock()))return;
	auto& uiManager = UIManager::GetInstance();
	uiManager.SetIsDraw(false);
}

void OptionScene::Init()
{
	//オプションUI作成
	auto optionUI = std::make_shared<OptionUI>();
	optionUI->Init();
	m_optionUI = optionUI;

	m_isWindowScreen = Application::GetInstance().IsWindowMode();
}

void OptionScene::Update()
{
	auto& input = Input::GetInstance();
	CameraController::GetInstance().Update();
	//更新
	(this->*m_update)(input);
}

void OptionScene::Draw()
{
}

void OptionScene::End()
{
}

void OptionScene::UpdateMenuSelect(Input& input)
{
	if (input.IsTrigger("B"))
	{
		//メインメニューへ戻る
		m_controller.PopScene();
		return;
	}

	//UI
	if (m_optionUI.expired())return;
	auto ui = m_optionUI.lock();

	//音量調整
	auto& soundManager = SoundManager::GetInstance();
	switch (m_currentOptionMenu)
	{
	case OptionScene::OptionMenu::BGM:
		if (input.IsRepeate("Right"))soundManager.LevelUpBGMVolume();
		if (input.IsRepeate("Left"))soundManager.LevelDownBGMVolume();
		break;
	case OptionScene::OptionMenu::SE:
		if (input.IsRepeate("Right"))soundManager.LevelUpSEVolume();
		if (input.IsRepeate("Left"))soundManager.LevelDownSEVolume();
		break;
	case OptionScene::OptionMenu::Voice:
		if (input.IsRepeate("Right"))soundManager.LevelUpVoiceVolume();
		if (input.IsRepeate("Left"))soundManager.LevelDownVoiceVolume();
		break;
	case OptionScene::OptionMenu::ScreenMode:
		if (input.IsTrigger("A"))
		{
			//UIに反映
			ui->SetIsWindowMode(true);
			//ウィンドウモード切り替え更新
			m_update = &OptionScene::UpdateScreenMode;
			return;
		}
		break;
	case OptionScene::OptionMenu::Back:
		if (input.IsTrigger("A"))
		{
			//メインメニューへ戻る
			m_controller.PopScene();
			return;
		}
		break;
	case OptionScene::OptionMenu::Max:
		break;
	default:
		break;
	}


	//メニュー操作
	int menu = static_cast<int>(m_currentOptionMenu);
	if (input.IsRepeate("Up"))menu--;
	if (input.IsRepeate("Down"))menu++;
	//範囲外ならループ
	if (menu < static_cast<int>(OptionMenu::BGM))menu = static_cast<int>(OptionMenu::Max) - 1;
	if (menu >= static_cast<int>(OptionMenu::Max))menu = static_cast<int>(OptionMenu::BGM);
	m_currentOptionMenu = static_cast<OptionMenu>(menu);

	//UIに反映
	ui->SetSelectMenuIndex(m_currentOptionMenu);
}

void OptionScene::UpdateScreenMode(Input& input)
{
	//UI
	if (m_optionUI.expired())return;
	auto ui = m_optionUI.lock();

	if(input.IsTrigger("B"))
	{
		//UIに反映
		ui->SetIsWindowMode(false);
		//スクリーンモード初期化
		m_isWindowScreen = Application::GetInstance().IsWindowMode();
		//メニュー選択更新へ戻る
		m_update = &OptionScene::UpdateMenuSelect;
		return;
	}

	//ウィンドウモード切り替え
	if (input.IsTrigger("A"))
	{
		if(m_isWindowScreen != Application::GetInstance().IsWindowMode())
		{
			//切り替え
			Application::GetInstance().SetWindowMode(m_isWindowScreen);
			return;
		}
	}

	//切り替え先を選ぶ
	if (input.IsTrigger("Up"))m_isWindowScreen = true;
	if (input.IsTrigger("Down"))m_isWindowScreen = false;

	//UIに反映
	ui->SetIsFullScreen(!m_isWindowScreen);
}
