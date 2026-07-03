#include "GameoverUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Game.h"
#include <DxLib.h>

namespace
{
	//パス
	const std::wstring kBackPath = L"Back/BlackBack1";
	const std::wstring kGameoverPath = L"Gameover/Gameover";
	const std::wstring kContiuePath = L"Gameover/Continue";
	const std::wstring kRestartPath = L"Gameover/Restart";
	const std::wstring kStageSelectPath = L"Result/SelectStage";
	const std::wstring kCursorPath = L"Mark/Cursor";

	//ゲームオーバー位置
	const Vector2 kGameoverFirstPos = Vector2(Game::kScreenCenterX, Game::kScreenCenterY);
	const Vector2 kGameoverSecondPos = Vector2(Game::kScreenCenterX, Game::kScreenCenterY - 200);
	//メニュー位置
	const Vector2 kMenuFirstPos = Vector2(-Game::kScreenCenterX, Game::kScreenCenterY - 60);
	const Vector2 kMenuSecondPos = Vector2(Game::kScreenCenterX, Game::kScreenCenterY - 60);

	//影の位置をずらす
	constexpr float kShadowOffsetX = 10.0f;
	constexpr float kShadowOffsetY = 10.0f;

	//カーソル位置
	constexpr float kCursorOffsetX = 350.0f;
	const Vector2 kCursorPos = Vector2(-Game::kScreenCenterX - kCursorOffsetX, Game::kScreenCenterY - 60.0f);

	//カーソル速度
	constexpr float kCursorSpeed = 10.0f;
	//カーソル移動幅
	constexpr float kCursorMoveWidth = 10.0f;

	//メニューテキストをずらす
	constexpr float kMenuOffsetY = 140.0f;

	//フレーム
	constexpr float kGameoverMoveFrame = 60.0f;
	constexpr float kMenuMoveFrame = 80.0f;

	//Lerp率
	constexpr float kGameoverLerpRate = 0.1f;
	constexpr float kMenuLerpRate = 0.1f;
	constexpr float kCursorLerpRateX = 0.2f;
	constexpr float kCursorLerpRateY = 0.5f;

}

GameoverUI::GameoverUI() :
	UIBase(),
	m_backHandle(-1),
	m_gameoverHandle(-1),
	m_countFrame(0),
	m_gameoverPos(kGameoverFirstPos),
	m_menuPos(kMenuFirstPos),
	m_cousorPos(kCursorPos),
	m_continueHandle(-1),
	m_restartHandle(-1),
	m_stageSelectHandle(-1),
	m_cursorHandle(-1),
	m_menuIndex(GameoverScene::Menu::Continue),
	m_isFinish(false)
{
	auto& assetManager = AssetManager::GetInstance();
	//背景
	m_backHandle = assetManager.GetImageHandle(kBackPath);
	//ゲームオーバー
	m_gameoverHandle = assetManager.GetImageHandle(kGameoverPath);
	//コンティニュー
	m_continueHandle = assetManager.GetImageHandle(kContiuePath);
	//リスタート
	m_restartHandle = assetManager.GetImageHandle(kRestartPath);
	//ステージセレクト
	m_stageSelectHandle = assetManager.GetImageHandle(kStageSelectPath);
	//カーソル
	m_cursorHandle = assetManager.GetImageHandle(kCursorPath);
}

GameoverUI::~GameoverUI()
{
}

void GameoverUI::Update()
{
	++m_countFrame;
	if (m_countFrame > kMenuMoveFrame)
	{
		m_menuPos = Vector2::Lerp(m_menuPos, kMenuSecondPos, kMenuLerpRate);
	}
	if (m_countFrame > kGameoverMoveFrame)
	{
		m_gameoverPos = Vector2::Lerp(m_gameoverPos, kGameoverSecondPos, kGameoverLerpRate);
		//メニューの座標
		Vector2 continuePos = m_menuPos;
		Vector2 restartPos = m_menuPos;
		restartPos.y += kMenuOffsetY;
		Vector2 stageSelectPos = m_menuPos;
		stageSelectPos.y += kMenuOffsetY * 2;
		//カーソルの座標
		Vector2 cousorPos = m_menuPos;
		switch (m_menuIndex)
		{
		case GameoverScene::Menu::Continue:
			cousorPos = continuePos;
			break;
		case GameoverScene::Menu::Restart:
			cousorPos = restartPos;
			break;
		case GameoverScene::Menu::StageSelect:
			cousorPos = stageSelectPos;
			break;
		default:
			break;
		}

		cousorPos.x -= kCursorOffsetX;

		m_cousorPos.x = MathSub::Lerp(m_cousorPos.x, cousorPos.x, kCursorLerpRateX);
		m_cousorPos.y = MathSub::Lerp(m_cousorPos.y, cousorPos.y, kCursorLerpRateY);
		m_isFinish = true;
	}
}

void GameoverUI::Draw() const
{
	//背景
	DrawGraph(0, 0, m_backHandle, true);
	//ゲームオーバー
	DrawRotaGraph(m_gameoverPos.x, m_gameoverPos.y, 1.0, 0.0, m_gameoverHandle, true);

	//メニューの座標
	Vector2 continuePos = m_menuPos;
	Vector2 restartPos = m_menuPos;
	restartPos.y += kMenuOffsetY;
	Vector2 stageSelectPos = m_menuPos;
	stageSelectPos.y += kMenuOffsetY * 2;

	//カーソルをずらす
	float cousorOffsetX = cosf(m_countFrame * kCursorSpeed * MyMath::DEG_2_RAD) * kCursorMoveWidth;

	//影
	SetDrawBlendMode(DX_BLENDMODE_SUB, 255);
	//コンティニュー
	DrawRotaGraphF(continuePos.x + kShadowOffsetX, continuePos.y + kShadowOffsetY, 1.0, 0.0, m_continueHandle, true);
	//リスタート
	DrawRotaGraphF(restartPos.x + kShadowOffsetX, restartPos.y + kShadowOffsetY, 1.0, 0.0, m_restartHandle, true);
	//ステージセレクト
	DrawRotaGraphF(stageSelectPos.x + kShadowOffsetX, stageSelectPos.y + kShadowOffsetY, 1.0, 0.0, m_stageSelectHandle, true);
	//カーソル
	DrawRotaGraphF(m_cousorPos.x + kShadowOffsetX + cousorOffsetX, m_cousorPos.y + kShadowOffsetY, 1.0, 0.0, m_cursorHandle, true);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	
	//コンティニュー
	DrawRotaGraphF(continuePos.x, continuePos.y, 1.0, 0.0, m_continueHandle, true);
	//リスタート
	DrawRotaGraphF(restartPos.x, restartPos.y, 1.0, 0.0, m_restartHandle, true);
	//ステージセレクト
	DrawRotaGraphF(stageSelectPos.x, stageSelectPos.y, 1.0, 0.0, m_stageSelectHandle, true);
	//選んでいるものを反転
	SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
	switch (m_menuIndex)
	{
	case GameoverScene::Menu::Continue:
		//コンティニュー
		DrawRotaGraphF(continuePos.x, continuePos.y, 1.0, 0.0, m_continueHandle, true);
		break;
	case GameoverScene::Menu::Restart:
		//リスタート
		DrawRotaGraphF(restartPos.x, restartPos.y, 1.0, 0.0, m_restartHandle, true);
		break;
	case GameoverScene::Menu::StageSelect:
		//ステージセレクト
		DrawRotaGraphF(stageSelectPos.x, stageSelectPos.y, 1.0, 0.0, m_stageSelectHandle, true);
		break;
	default:
		break;
	}
	//カーソル
	DrawRotaGraphF(m_cousorPos.x + cousorOffsetX, m_cousorPos.y, 1.0, 0.0, m_cursorHandle, true);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	
}
