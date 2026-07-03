#include "TutorialResultUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Game.h"
#include <DxLib.h>

namespace
{
	//パス
	const std::wstring kMCPath = L"Result/MissionComplete";
	const std::wstring kBackPath = L"Back/BeigeBack";
	const std::wstring kNextTutorialPath = L"Result/NextTutorial";
	const std::wstring kSelectStagePath = L"Result/SelectStage";
	const std::wstring kCursorPath = L"Mark/Cursor";
	//終了フレーム
	constexpr int kFinishFrame = 45;
	//位置
	const Vector2 kMCFirstPos = Vector2(Game::kScreenCenterX, Game::kScreenCenterY);
	const Vector2 kMCSecondPos = Vector2(Game::kScreenCenterX, Game::kScreenCenterY - 200);
	const Vector2 kMenuSecondPos = Vector2(Game::kScreenCenterX, Game::kScreenCenterY);
	constexpr int kMenuOffsetY = 150;
	constexpr int kCousorPosX = 300;
	//Lerp
	constexpr float kMCLerpRate = 0.1f;
	constexpr float kMenuLerpRate = 0.1f;
	constexpr float kCousorLerpXRate = 0.1f;
	constexpr float kCousorLerpYRate = 0.5f;
	//影の位置をずらす
	constexpr float kShadowOffsetX = 10.0f;
	constexpr float kShadowOffsetY = 10.0f;
	//カーソル速度
	constexpr float kCousorSpeed = 10.0f;
	//カーソル移動幅
	constexpr float kCousorMoveWidth = 10.0f;
}

TutorialResultUI::TutorialResultUI(StageIndex tutorialIndex) :
	UIBase(),
	m_isFinish(false),
	m_mcHandle(-1),
	m_backHandle(-1),
	m_nextTutorialHandle(-1),
	m_selectStageHandle(-1),
	m_countFrame(0),
	m_mcPos{ -Game::kScreenWidth,Game::kScreenCenterY },
	m_menuPos{ -Game::kScreenWidth, Game::kScreenCenterY },
	m_cousorPos{ -Game::kScreenWidth, Game::kScreenCenterY },
	m_menuIndex(TutorialResultScene::Menu::Next)
{
	auto& assetManager = AssetManager::GetInstance();
	//ミッションコンプリート
	m_mcHandle = assetManager.GetImageHandle(kMCPath);
	//背景
	m_backHandle = assetManager.GetImageHandle(kBackPath);
	//セレクト
	m_nextTutorialHandle = assetManager.GetImageHandle(kNextTutorialPath);
	m_selectStageHandle = assetManager.GetImageHandle(kSelectStagePath);
	//カーソル
	m_cursorHandle = assetManager.GetImageHandle(kCursorPath);
}

TutorialResultUI::~TutorialResultUI()
{
}

void TutorialResultUI::Update()
{
	++m_countFrame;
	if (m_countFrame <= kFinishFrame)
	{
		m_mcPos = Vector2::Lerp(m_mcPos, kMCFirstPos, kMCLerpRate);
	}
	else
	{
		m_mcPos = Vector2::Lerp(m_mcPos, kMCSecondPos, kMCLerpRate);
		m_menuPos = Vector2::Lerp(m_menuPos, kMenuSecondPos, kMenuLerpRate);

		Vector2 nextCousorPos = m_menuPos;
		Vector2 nextTutorialPos = m_menuPos;
		Vector2 selectStagePos = m_menuPos;
		selectStagePos.y += kMenuOffsetY;
		switch (m_menuIndex)
		{
		case TutorialResultScene::Menu::Next:
			nextCousorPos = nextTutorialPos;
			break;
		case TutorialResultScene::Menu::Select:
			nextCousorPos = selectStagePos;
			break;
		default:
			break;
		}
		nextCousorPos.x = kCousorPosX;
		m_cousorPos.x = MathSub::Lerp(m_cousorPos.x, nextCousorPos.x, kCousorLerpXRate);
		m_cousorPos.y = MathSub::Lerp(m_cousorPos.y, nextCousorPos.y, kCousorLerpYRate);
		m_isFinish = true;
	}
}

void TutorialResultUI::Draw() const
{	
	//背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	DrawGraph(0, 0, m_backHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//ミッションコンプリート
	DrawRotaGraph(m_mcPos.x, m_mcPos.y, 1.0, 0.0, m_mcHandle, true);

	//セレクト
	Vector2 nextTutorialPos = m_menuPos;
	Vector2 selectStagePos = m_menuPos;
	selectStagePos.y += kMenuOffsetY;

	//カーソルを動かす
	float cousorOffsetPosX = cosf(m_countFrame * kCousorSpeed * MyMath::DEG_2_RAD) * kCousorMoveWidth;

	//影
	SetDrawBlendMode(DX_BLENDMODE_SUB, 255);
	DrawRotaGraph(nextTutorialPos.x + kShadowOffsetX, nextTutorialPos.y + kShadowOffsetY, 1.0, 0.0, m_nextTutorialHandle, true);
	DrawRotaGraph(selectStagePos.x + kShadowOffsetX, selectStagePos.y + kShadowOffsetY, 1.0, 0.0, m_selectStageHandle, true);
	DrawRotaGraph(m_cousorPos.x + kShadowOffsetX + cousorOffsetPosX, m_cousorPos.y + kShadowOffsetY, 1.0, 0.0, m_cursorHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawRotaGraph(nextTutorialPos.x, nextTutorialPos.y, 1.0, 0.0, m_nextTutorialHandle, true);
	DrawRotaGraph(selectStagePos.x, selectStagePos.y , 1.0, 0.0, m_selectStageHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
	switch (m_menuIndex)
	{
	case TutorialResultScene::Menu::Next:
		DrawRotaGraph(nextTutorialPos.x, nextTutorialPos.y, 1.0, 0.0, m_nextTutorialHandle, true);
		break;
	case TutorialResultScene::Menu::Select:
		DrawRotaGraph(selectStagePos.x, selectStagePos.y, 1.0, 0.0, m_selectStageHandle, true);
		break;
	default:
		break;
	}
	//カーソル
	DrawRotaGraph(m_cousorPos.x + cousorOffsetPosX, m_cousorPos.y, 1.0, 0.0, m_cursorHandle, true);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	
}
