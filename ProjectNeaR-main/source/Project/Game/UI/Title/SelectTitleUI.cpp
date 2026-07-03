#include "SelectTitleUI.h"
#include "SaveDataUI.h"
#include "../../../General/AssetManager.h"
#include <string>

namespace
{
	//パス
	const std::wstring kSelectContinue = L"Title/SelectContinue";
	const std::wstring kSelectNewGame = L"Title/SelectNewGame";
	const std::wstring kSelectOption = L"Title/SelectOption";
	const std::wstring kSelectGameEnd = L"Title/SelectGameEnd";
	const std::wstring kMenuShadow = L"Title/Menu_Shadow";
	const std::wstring kCursor = L"Mark/Cursor";
	const std::wstring kCursorShadow = L"Mark/Cursor_Shadow";

	//画面外座標
	constexpr float kOutPosX = -200.0f;

	//待機座標
	constexpr float kWaitPosX = 300.0f;

	//選択中の座標
	constexpr float kSelectPosX = kWaitPosX + 50.0f;
	
	//各メニュー座標
	constexpr float kContinuePosY = 430.0f;
	constexpr float kNewGamePosY = kContinuePosY + 65.0f;
	constexpr float kOptionPosY = kNewGamePosY + 65.0f;
	constexpr float kGameEndPosY = kOptionPosY + 65.0f;

	//影
	constexpr float kShadowOffsetPosX = 10.0f;
	constexpr float kShadowOffsetPosY = 10.0f;

	//カーソル
	constexpr float kCursorPosX = 120.0f;

	//Lerp
	constexpr float kLerpRate = 0.2f;
}

SelectTitleUI::SelectTitleUI():
	UIBase(),
	m_selectIndex(TitleScene::SelectMenuTitle::Continue),
	m_cursorHandle(-1),
	m_shadowMenuHandle(-1),
	m_shadowCursorHandle(-1),
	m_cursorPos()
{
	auto& assetManager = AssetManager::GetInstance();
	//ハンドル
	m_menuHandle.emplace_back(assetManager.GetImageHandle(kSelectContinue));
	m_menuHandle.emplace_back(assetManager.GetImageHandle(kSelectNewGame));
	m_menuHandle.emplace_back(assetManager.GetImageHandle(kSelectOption));
	m_menuHandle.emplace_back(assetManager.GetImageHandle(kSelectGameEnd));
	m_cursorHandle = assetManager.GetImageHandle(kCursor);
	m_shadowMenuHandle = assetManager.GetImageHandle(kMenuShadow);
	m_shadowCursorHandle = assetManager.GetImageHandle(kCursorShadow);

	//座標
	m_menuPoses.emplace_back(Vector2(kWaitPosX, kContinuePosY));
	m_menuPoses.emplace_back(Vector2(kWaitPosX, kNewGamePosY));
	m_menuPoses.emplace_back(Vector2(kWaitPosX, kOptionPosY));
	m_menuPoses.emplace_back(Vector2(kWaitPosX, kGameEndPosY));
	m_cursorPos = m_menuPoses.front();

	//セーブデータUI
	auto saveDataUI = std::make_shared<SaveDataUI>();
	saveDataUI->Init();
	m_saveDataUI = saveDataUI;
}

SelectTitleUI::~SelectTitleUI()
{
	if (m_saveDataUI.expired())return;
	m_saveDataUI.lock()->Delete();
}

void SelectTitleUI::Update()
{
	if (m_saveDataUI.expired())return;
	auto saveDataUI = m_saveDataUI.lock();

	if (m_isDraw)
	{
		//画面外から登場
		for (int i = 0; i < m_menuPoses.size(); ++i)
		{
			//選択中の項目
			if (static_cast<int>(m_selectIndex) == i)
			{
				m_menuPoses[i].x = MathSub::Lerp(m_menuPoses[i].x, kSelectPosX, kLerpRate);
				m_cursorPos.x = MathSub::Lerp(m_cursorPos.x, kCursorPosX, kLerpRate);
				m_cursorPos.y = MathSub::Lerp(m_cursorPos.y, m_menuPoses[i].y, kLerpRate);
				continue;
			}
			m_menuPoses[i].x = MathSub::Lerp(m_menuPoses[i].x, kWaitPosX, kLerpRate);
		}
		saveDataUI->EnableDraw();
	}
	else
	{
		//描画してない間は画面外で待機
		for (auto& pos : m_menuPoses)
		{
			pos.x = kOutPosX;
		}
		m_cursorPos.x = kOutPosX;
		saveDataUI->DisableDraw();
	}
}

void SelectTitleUI::Draw() const
{
	if (!m_isDraw)return;

	const int kContinue = static_cast<int>(TitleScene::SelectMenuTitle::Continue);
	const int kNewGame = static_cast<int>(TitleScene::SelectMenuTitle::NewGame);
	const int kOption = static_cast<int>(TitleScene::SelectMenuTitle::Option);
	const int kGameEnd = static_cast<int>(TitleScene::SelectMenuTitle::GameEnd);

	//影
	DrawRotaGraphF(m_menuPoses[kContinue].x + kShadowOffsetPosX, m_menuPoses[kContinue].y + kShadowOffsetPosY, 1.0, 0.0, m_shadowMenuHandle, true);
	DrawRotaGraphF(m_menuPoses[kNewGame].x + kShadowOffsetPosX, m_menuPoses[kNewGame].y + kShadowOffsetPosY, 1.0, 0.0, m_shadowMenuHandle, true);
	DrawRotaGraphF(m_menuPoses[kOption].x + kShadowOffsetPosX, m_menuPoses[kOption].y + kShadowOffsetPosY, 1.0, 0.0, m_shadowMenuHandle, true);
	DrawRotaGraphF(m_menuPoses[kGameEnd].x + kShadowOffsetPosX, m_menuPoses[kGameEnd].y + kShadowOffsetPosY, 1.0, 0.0, m_shadowMenuHandle, true);
	DrawRotaGraphF(m_cursorPos.x + kShadowOffsetPosX, m_cursorPos.y + kShadowOffsetPosY, 1.0, 0.0, m_shadowCursorHandle, true);

	//描画
	DrawRotaGraphF(m_menuPoses[kContinue].x, m_menuPoses[kContinue].y, 1.0, 0.0, m_menuHandle[kContinue], true);
	DrawRotaGraphF(m_menuPoses[kNewGame].x, m_menuPoses[kNewGame].y, 1.0, 0.0, m_menuHandle[kNewGame], true);
	DrawRotaGraphF(m_menuPoses[kOption].x, m_menuPoses[kOption].y, 1.0, 0.0, m_menuHandle[kOption], true);
	DrawRotaGraphF(m_menuPoses[kGameEnd].x, m_menuPoses[kGameEnd].y, 1.0, 0.0, m_menuHandle[kGameEnd], true);

	//選択中なら反転
	const int kSelectIndex = static_cast<int>(m_selectIndex);
	SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);

	//選択中のメニュー
	DrawRotaGraphF(m_menuPoses[kSelectIndex].x, m_menuPoses[kSelectIndex].y, 1.0, 0.0, m_menuHandle[kSelectIndex], true);

	//カーソル
	DrawRotaGraphF(m_cursorPos.x, m_cursorPos.y, 1.0, 0.0, m_cursorHandle, true);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

}
