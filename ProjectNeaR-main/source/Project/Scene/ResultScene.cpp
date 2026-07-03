#include "ResultScene.h"
#include "SelectScene.h"
#include "EndingScene.h"
#include <Dxlib.h>
#include  "../General/Input.h"
#include  "../General/Timer.h"
#include  "../General/Fader.h"
#include  "../General/Game.h"
#include "SceneController.h"
#include "GameScene.h"
#include "../Game/UI/Result/ResultUI.h"
#include "../Game/UI/UIManager.h"
#include "../Game/Camera/CameraController.h"
#include "../General/AssetManager.h"
#include "../General/Sound/SoundManager.h"
#include "../General/Effect/EffekseerManager.h"
#include "../General/SaveDataManager.h"
#include "../General/CSV/ClearSaveData.h"
#include "../General/CSV/HighScoreData.h"

namespace
{
	constexpr unsigned int kFadeColor = 0x000000;

	//BGM
	const std::wstring kBGMResultPath = L"ResultScene";
}

ResultScene::ResultScene(std::wstring stageName, SceneController& controller, std::shared_ptr<Timer> timer) :
	SceneBase(controller),
	m_resultUI(),
	m_timer(timer),
	m_stageName(stageName)
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init()
{
	//フェードイン
	Fader::GetInstance().FadeIn();

	//アセット削除
	AssetManager::GetInstance().AllDelete();

	//エフェクトリセット
	EffekseerManager::GetInstance().Reset();
	
	auto& uiManager = UIManager::GetInstance();
	//UIの削除
	uiManager.AllDeleteUI();
	//UI表示
	uiManager.SetIsDraw(true);

	//カメラリセット
	CameraController::GetInstance().Init();

	//音リセット
	auto& soundManager = SoundManager::GetInstance();
	soundManager.Reset();
	//BGM
	soundManager.PlayBGM(kBGMResultPath);


	//入力を開始
	Input::GetInstance().StartUpdate();

	//リザルトUI
	auto resultUI = std::make_shared<ResultUI>(m_stageName,m_timer);
	resultUI->Init();
	m_resultUI = resultUI;

	//ステージインデックス
	StageIndex index = GetStageIndexByName(m_stageName);

	//セーブ
	auto& saveDataManager = SaveDataManager::GetInstance();
	//ステージクリア
	auto clearData = saveDataManager.GetClearData();

	//ステージをクリアしたことを記録
	switch (index)
	{
	case StageIndex::Stage1:
		clearData->ClearStage1();
		break;
	case StageIndex::Stage2:
		clearData->ClearStage2();
		break;
	case StageIndex::Stage3:
		clearData->ClearStage3();
		break;
	default:
		break;
	}

}

void ResultScene::Update()
{
	//プレイ時間加算
	SaveDataManager::GetInstance().AddPlayTime();

	auto& input = Input::GetInstance();
	auto& fader = Fader::GetInstance();
	//フェードアウトしきったら
	if (fader.IsFinishFadeOut())
	{
		//最終ステージならエンディングへ
		if (GetStageIndexByName(m_stageName) == StageIndex::Stage3)
		{
			m_controller.ChangeScene(std::make_unique<EndingScene>(m_controller));
			return;
		}
		m_controller.ChangeScene(std::make_unique<SelectScene>(m_controller));
		return;
	}
	//ボタンを押したら次
	if (input.IsTrigger("A") && !fader.IsFadeNow() && !m_resultUI.expired())
	{
		if (m_resultUI.lock()->IsFinish())
		{
			//フェードカラー
			fader.SetColor(kFadeColor);
			//フェード
			fader.FadeOut();
		}
	}

}

void ResultScene::Draw()
{
}

void ResultScene::End()
{
}

void ResultScene::DebugDraw() const
{
#if _DEBUG
	DrawString(0, 0, L"Result Scene", 0xffffff);
	DrawString(0, 16, L"[D]キーで Debug Scene", 0xffffff);
#endif
}
