#include "EndingScene.h"
#include <Dxlib.h>
#include  "../General/Input.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "DebugScene/DebugSelectScene.h"
#include "../General/Fader.h"
#include "../General/SaveDataManager.h"
#include "../General/Game.h"
#include "../Game/UI/Ending/EndingTextUI.h"
#include "../Game/UI/SkipUI.h"
#include "../Game/UI/UIManager.h"
#include "../Game/Actor/ActorManager.h"
#include "../General/AssetManager.h"
#include "../Game/Camera/CameraController.h"
#include "../Game/Camera/TitleCamera.h"
#include "../Main/Application.h"
#include "../General/ShaderPostProcess.h"
#include "../General/Math/MyMath.h"
#include "../General/Effect/EffekseerManager.h"
#include "../General/Sound/SoundManager.h"
#include "../General/Sound/SE.h"

namespace
{
	//タイトル
	const std::wstring kEndingPath = L"Ending";
	const std::wstring kCharacterDataPath = L"CharacterData";
	const std::wstring kStageDataPath = L"StageData";

	//BGM
	const std::wstring kBGMEndingPath = L"TitleScene";

	//スキップフレーム数
	constexpr int kSkipFrame = 60 * 2;

	//終了フレーム
	constexpr int kFinishFrame = 60 * 3;
}

EndingScene::EndingScene(SceneController& controller) :
	SceneBase(controller),
	m_effectManager(EffekseerManager::GetInstance()),
	m_skipCountFrame(0),
	m_finishCountFrame(0)
{
}

EndingScene::~EndingScene()
{
}

void EndingScene::Init()
{
	//アセットの削除
	AssetManager::GetInstance().AllDelete();
	//タイムスケール
	Application::GetInstance().SetTimeScale(1.0f);
	//UI削除
	UIManager::GetInstance().AllDeleteUI();
	//フェードイン
	Fader::GetInstance().FadeIn();
	//シェーダーリセット
	Application::GetInstance().GetPostProcess()->ResetPostEffectState();
	//エフェクトマネージャー初期化
	m_effectManager.Init();
	//音
	auto& soundManager = SoundManager::GetInstance();
	soundManager.Reset();
	//サウンドデータのロード
	soundManager.LoadBGM(kBGMEndingPath);
	//BGM再生
	soundManager.PlayBGM(kBGMEndingPath);
	
	//カメラ
	auto camera = std::make_shared<TitleCamera>();
	auto& cameraController = CameraController::GetInstance();
	cameraController.Init();
	cameraController.ChangeCamera(camera);

	//アクターマネージャー
	m_actorManager = std::make_shared<ActorManager>();
	m_actorManager->Init();
	m_actorManager->CreateActorCSV(kEndingPath.c_str(), kCharacterDataPath.c_str());
	m_actorManager->CreateActorCSV(kEndingPath.c_str(), kStageDataPath.c_str());

	//テキストUI
	auto endingTextUI = std::make_shared<EndingTextUI>();
	endingTextUI->Init();
	m_endingTextUI = endingTextUI;

	//スキップUI
	auto skipUI = std::make_shared<SkipUI>();
	skipUI->Init();
	m_skipUI = skipUI;

	//スキップフレーム
	m_skipCountFrame = 0;
	//終了フレーム
	m_finishCountFrame = 0;

	//セーブ
	SaveDataManager::GetInstance().SaveAllData();
}

void EndingScene::Update()
{
	//プレイ時間加算
	SaveDataManager::GetInstance().AddPlayTime();

	auto& input = Input::GetInstance();

	//エフェクトの再生
	m_effectManager.Update();

	auto& fader = Fader::GetInstance();
	//フェードアウトしきったら
	if (fader.IsFinishFadeOut())
	{
		m_controller.ChangeScene(std::make_unique<TitleScene>(m_controller));
		return;
	}

	//カメラ
	auto& cameraController = CameraController::GetInstance();
	cameraController.Update();

	//全ての処理が終了したら
	if (!m_endingTextUI.expired())
	{
		if (m_endingTextUI.lock()->IsAllEnd())
		{
			++m_finishCountFrame;
		}
		m_finishCountFrame = MathSub::ClampInt(m_finishCountFrame, 0, kFinishFrame);
	}

	//スキップ処理
	if (!m_skipUI.expired())
	{
		if (input.IsPress("A"))
		{
			++m_skipCountFrame;
		}
		else
		{
			--m_skipCountFrame;
		}
		m_skipCountFrame = MathSub::ClampInt(m_skipCountFrame, 0, kSkipFrame);
		//割合
		m_skipUI.lock()->SetRate(static_cast<float>(m_skipCountFrame) / static_cast<float>(kSkipFrame));
	}

	//終了条件
	if ((m_skipCountFrame >= kSkipFrame || m_finishCountFrame >= kFinishFrame) && !fader.IsFadeNow())
	{
		//フェード
		fader.FadeOut();
	}

	m_actorManager->Update();
}

void EndingScene::Draw()
{
	m_actorManager->Draw();
	m_effectManager.Draw();
}

void EndingScene::End()
{
	m_actorManager->End();
}

void EndingScene::DebugDraw() const
{
#if _DEBUG
	DrawString(0, 0, L"Ending Scene", 0xffffff);
	DrawString(0, 16, L"[D]キーで Debug Scene", 0xffffff);
#endif
}
