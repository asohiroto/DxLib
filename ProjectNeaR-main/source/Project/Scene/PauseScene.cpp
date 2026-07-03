#include "PauseScene.h"
#include "SelectScene.h"
#include "OptionScene.h"
#include "../General/Input.h"
#include "SceneController.h"
#include<DxLib.h>
#include "../General/game.h"
#include "../General/Collision/Physics.h"
#include "../General/Sound/SoundManager.h"
#include "../Game/UI/UIManager.h"
#include "../General/Effect/EffekseerManager.h"
#include "../General/AssetManager.h"
#include "../General/Fader.h"
#include "../General/SaveDataManager.h"
#include "../Game/Camera/CameraController.h"

namespace
{
	//パス
	const std::wstring kBackGame = L"Pause/BackGame";
	const std::wstring kRestartGame = L"Pause/RestartGame";
	const std::wstring kOption = L"Pause/Option";
	const std::wstring kReturn = L"Pause/Return";
	const std::wstring kBack = L"Pause/PauseBack";
	const std::wstring kCursor = L"Mark/Cursor";

	//座標
	const Vector2 kBackPos = { Game::kScreenCenterX, Game::kScreenCenterY };
	const Vector2 kBackGamePos = { Game::kScreenCenterX, 230.0f };
	const Vector2 kRestartGamePos = { Game::kScreenCenterX, 330.0f };
	const Vector2 kOptionPos = { Game::kScreenCenterX, 430.0f };
	const Vector2 kReturnPos = { Game::kScreenCenterX, 530.0f };
	//カーソル座標
	constexpr float kCursorOffset = -250.0f;
	const Vector2 kCursorBackGamePos = { kBackGamePos.x + kCursorOffset, kBackGamePos.y };
	const Vector2 kCursorRestartGamePos = { kRestartGamePos.x + kCursorOffset, kRestartGamePos.y };
	const Vector2 kCursorOptionPos = { kOptionPos.x + kCursorOffset, kOptionPos.y };
	const Vector2 kCursorReturnPos = { kReturnPos.x + kCursorOffset, kReturnPos.y };
	constexpr float kCursorRange = 20.0f;
	//カーソル角度加算速度
	constexpr float kCursorAngleSpeed = 5.0f;

	//カーソルLerp率
	const float kCursorLerpRate = 0.3f;

	//影
	const Vector2 kShadowOffsetPos = { 10.0f,10.0f };

	//SE
	const std::wstring kSESelectPath = L"Select";
	const std::wstring kSEOKPath = L"OK";
	const std::wstring kSECancelPath = L"Cancel";
}

PauseScene::PauseScene(SceneController& controller) :
	SceneBase(controller),
	m_backGameHandle(-1),
	m_restartGameHandle(-1),
	m_optionHandle(-1),
	m_returnHandle(-1),
	m_backHandle(-1),
	m_cursorHandle(-1),
	m_cursorPos{ kCursorBackGamePos },
	m_menuIndex(MenuIndex::BackGame),
	m_cursorAngle(0.0f)
{
}

PauseScene::~PauseScene()
{
	auto& input = Input::GetInstance();
	//入力をリセット
	input.ResetChangeScene();
}

void PauseScene::Init()
{
	//描画停止
	UIManager::GetInstance().SetIsDraw(false);
	//エフェクト停止
	EffekseerManager::GetInstance().StopEffect();
	//Physicsを止める
	Physics::GetInstance().StopUpdate();
	//画像ハンドル
	auto& assetManager = AssetManager::GetInstance();
	m_backGameHandle = assetManager.GetImageHandle(kBackGame);
	m_restartGameHandle = assetManager.GetImageHandle(kRestartGame);
	m_optionHandle = assetManager.GetImageHandle(kOption);
	m_returnHandle = assetManager.GetImageHandle(kReturn);
	m_backHandle = assetManager.GetImageHandle(kBack);
	m_cursorHandle = assetManager.GetImageHandle(kCursor);
	//サウンド
	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSE(kSESelectPath);
	soundManager.LoadSE(kSEOKPath);
	soundManager.LoadSE(kSECancelPath);
}

void PauseScene::Update()
{
	//プレイ時間加算
	SaveDataManager::GetInstance().AddPlayTime();

	//カメラの更新
	CameraController::GetInstance().Update();

	//サウンド
	auto& soundManager = SoundManager::GetInstance();

	//フェード
	auto& fader = Fader::GetInstance();

	//UIマネージャー
	auto& uiManager = UIManager::GetInstance();
	uiManager.SetIsDraw(false);

	auto& input = Input::GetInstance();

	//フェード終了
	if (fader.IsFinishFadeOut())
	{
		//ステージセレクトへ戻る
		m_controller.ChangeBaseScene(std::make_shared<SelectScene>(m_controller));
		m_controller.PopScene();
	}

	if ((input.IsTrigger("Pause") || input.IsTrigger("B")) && !fader.IsFadeNow())
	{
		//SE再生
		soundManager.PlayOnceSE(kSECancelPath);
		//ゲームに戻る
		m_menuIndex = MenuIndex::BackGame;
		m_controller.PopScene();
		return;
	}

	//決定処理
	if (input.IsTrigger("A") && !fader.IsFadeNow())
	{
		//SE再生
		soundManager.PlayOnceSE(kSEOKPath);
		switch (m_menuIndex)
		{
		case PauseScene::MenuIndex::BackGame:
			//ゲームに戻る
			m_controller.PopScene();
			break;
		case PauseScene::MenuIndex::RestartGame:
			//ゲームを最初から
			if (!m_controller.GetBaseScene().expired())
			{
				auto baseScene = m_controller.GetBaseScene().lock();
				baseScene->End();	//終了処理
				baseScene->Init();	//初期化
				m_controller.PopScene();
			}
			break;
		case PauseScene::MenuIndex::Option:
			//UI描画再開
			uiManager.SetIsDraw(true);
			//設定
			m_controller.PushScene(std::make_shared<OptionScene>(m_controller));
			break;
		case PauseScene::MenuIndex::Return:
			fader.FadeOut();
			break;
		default:
			break;
		}
		return;
	}

	//メニューセレクト
	int menu = static_cast<int>(m_menuIndex);
	if (input.IsRepeate("Up"))menu--;
	if (input.IsRepeate("Down"))menu++;
	if (menu < static_cast<int>(MenuIndex::BackGame))
	{
		menu = static_cast<int>(MenuIndex::Return);
	}
	if (menu > static_cast<int>(MenuIndex::Return))
	{
		menu = static_cast<int>(MenuIndex::BackGame);
	}
	//カーソルを動かしたら
	if (menu != static_cast<int>(m_menuIndex))
	{
		//SE再生
		soundManager.PlayOnceSE(kSESelectPath);
	}

	m_menuIndex = static_cast<MenuIndex>(menu);

	switch (m_menuIndex)
	{
	case PauseScene::MenuIndex::BackGame:
		m_cursorPos = Vector2::Lerp(m_cursorPos, kCursorBackGamePos, kCursorLerpRate);
		break;
	case PauseScene::MenuIndex::RestartGame:
		m_cursorPos = Vector2::Lerp(m_cursorPos, kCursorRestartGamePos, kCursorLerpRate);
		break;
	case PauseScene::MenuIndex::Option:
		m_cursorPos = Vector2::Lerp(m_cursorPos, kCursorOptionPos, kCursorLerpRate);
		break;
	case PauseScene::MenuIndex::Return:
		m_cursorPos = Vector2::Lerp(m_cursorPos, kCursorReturnPos, kCursorLerpRate);
		break;
	default:
		break;
	}

	//角度加算
	m_cursorAngle += kCursorAngleSpeed;
	m_cursorAngle = static_cast<int>(m_cursorAngle) % 360;
}

void PauseScene::Draw()
{
	//自分がシーンの一番上にあるなら描画
	if (m_controller.GetTopScene().expired())return;
	if (!std::dynamic_pointer_cast<PauseScene>(m_controller.GetTopScene().lock()))return;

	//ゲームに戻る
	DrawRotaGraph(kBackPos.x, kBackPos.y, 1.0, 0.0, m_backHandle, true);

	//カーソルをずらす
	float cursorMoveX = cosf(m_cursorAngle * MyMath::DEG_2_RAD) * kCursorRange;
	//影
	SetDrawBlendMode(DX_BLENDMODE_SUB, 255);
	DrawRotaGraph(kBackGamePos.x + kShadowOffsetPos.x, kBackGamePos.y + kShadowOffsetPos.y, 1.0, 0.0, m_backGameHandle, true);
	DrawRotaGraph(kRestartGamePos.x + kShadowOffsetPos.x, kRestartGamePos.y + kShadowOffsetPos.y, 1.0, 0.0, m_restartGameHandle, true);
	DrawRotaGraph(kOptionPos.x + kShadowOffsetPos.x, kOptionPos.y + kShadowOffsetPos.y, 1.0, 0.0, m_optionHandle, true);
	DrawRotaGraph(kReturnPos.x + kShadowOffsetPos.x, kReturnPos.y + kShadowOffsetPos.y, 1.0, 0.0, m_returnHandle, true);
	DrawRotaGraphF(m_cursorPos.x + kShadowOffsetPos.x + cursorMoveX, m_cursorPos.y + kShadowOffsetPos.y, 1.0, 0.0, m_cursorHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	//ゲームに戻る
	DrawRotaGraphF(kBackGamePos.x, kBackGamePos.y, 1.0, 0.0, m_backGameHandle, true);
	//最初から
	DrawRotaGraphF(kRestartGamePos.x, kRestartGamePos.y, 1.0, 0.0, m_restartGameHandle, true);
	//設定
	DrawRotaGraphF(kOptionPos.x, kOptionPos.y, 1.0, 0.0, m_optionHandle, true);
	//撤退
	DrawRotaGraphF(kReturnPos.x, kReturnPos.y, 1.0, 0.0, m_returnHandle, true);
	
	//選んでいるものを反転
	SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
	switch (m_menuIndex)
	{
	case PauseScene::MenuIndex::BackGame:
		DrawRotaGraphF(kBackGamePos.x, kBackGamePos.y, 1.0, 0.0, m_backGameHandle, true);
		break;
	case PauseScene::MenuIndex::RestartGame:
		DrawRotaGraphF(kRestartGamePos.x, kRestartGamePos.y, 1.0, 0.0, m_restartGameHandle, true);
		break;
	case PauseScene::MenuIndex::Option:
		DrawRotaGraphF(kOptionPos.x, kOptionPos.y, 1.0, 0.0, m_optionHandle, true);
		break;
	case PauseScene::MenuIndex::Return:
		DrawRotaGraphF(kReturnPos.x, kReturnPos.y, 1.0, 0.0, m_returnHandle, true);
		break;
	default:
		break;
	}
	
	//カーソル
	DrawRotaGraphF(m_cursorPos.x + cursorMoveX, m_cursorPos.y, 1.0, 0.0, m_cursorHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void PauseScene::End()
{
	//再スタート以外の処理なら呼ぶ
	if (m_menuIndex != MenuIndex::RestartGame)
	{
		//Physicsを開始
		Physics::GetInstance().StartUpdate();
		//エフェクト開始
		EffekseerManager::GetInstance().StartEffect();
		//描画停止
		UIManager::GetInstance().SetIsDraw(true);
	}
}