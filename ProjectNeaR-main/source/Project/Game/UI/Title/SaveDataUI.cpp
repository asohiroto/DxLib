#include "SaveDataUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Game.h"
#include "../../../General/SaveDataManager.h"
#include "../../../General/CSV/TimeSaveData.h"
#include "../../../General/CSV/ClearSaveData.h"
#include <DxLib.h>

namespace
{
	//位置
	const Vector2 kNormalPos = { Game::kScreenCenterX + 200.0f,Game::kScreenCenterY + 160.0f };
	const Vector2 kOutPos = { Game::kScreenWidth + 1000.0f,Game::kScreenCenterY + 160.0f };

	//位置調整
	constexpr float kSaveDataTextOffsetPosX = -75.0f;
	constexpr float kSaveDataTextOffsetPosY = -94.0f;
	constexpr float kSaveDataNameTextOffsetPosX = -190.0f;
	constexpr float kSaveDataNameTextOffsetPosY = -60.0f;
	constexpr float kClearRateTextOffsetPosX = -190.0f;
	constexpr float kClearRateTextOffsetPosY = -20.0f;
	constexpr float kPlayTimeTextOffsetPosX = -190.0f;
	constexpr float kPlayTimeTextOffsetPosY = 20.0f;
	constexpr float kLastPlayDateTextOffsetPosX = -190.0f;
	constexpr float kLastPlayDateTextOffsetPosY = 60.0f;

	//Lerp率
	constexpr float kLerpRate = 0.2f;

	//影
	constexpr float kShadowOffsetX = 10.0f;
	constexpr float kShadowOffsetY = 10.0f;

	//パス
	const std::wstring kBackImagePath = L"Back/SaveDataBack";
	const std::wstring kBackShadowImagePath = L"Back/SaveDataBack_Shadow";

	//セーブデータテキスト
	const std::wstring kSaveDataText = L"< SAVEDATA >";
	//名前(固定)
	const std::wstring kSaveDataNameText = L"CODE : NeaR";
	//進行度
	const std::wstring kClearRateText = L"任務進行度 : ";
	//進行度(割合)
	const std::wstring kClearRate0 = L"0%";
	const std::wstring kClearRate1 = L"33%";
	const std::wstring kClearRate2 = L"72%";
	const std::wstring kClearRate3 = L"100%";
	//総プレイ時間
	const std::wstring kPlayTimeText = L"プレイ時間 : ";
	//最後のプレイ
	const std::wstring kLastPlayDateText = L"前回の記録日 : ";

	//時間
	const std::wstring kHour = L"時間";
	//分
	const std::wstring kMin = L"分";
}

SaveDataUI::SaveDataUI():
	m_pos(kNormalPos),
	m_backHandle(-1),
	m_backShadowHandle(-1),
	m_fontHandle(-1),
	m_clearRateText(L""),
	m_totalPlayTimeText(L""),
	m_lastPlayDateText(L"")
{
	auto& assetManager = AssetManager::GetInstance();
	m_backHandle = assetManager.GetImageHandle(kBackImagePath);
	m_backShadowHandle = assetManager.GetImageHandle(kBackShadowImagePath);
	m_fontHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::NotoSansJP, AssetManager::FontSize::Size24));

	//総プレイ時間
	auto& saveDataManager = SaveDataManager::GetInstance();
	auto timeData = saveDataManager.GetTimeSaveData();
	m_totalPlayTimeText = kPlayTimeText +
		std::to_wstring(timeData->GetPlayTimeHour()) + kHour +
		std::to_wstring(timeData->GetPlayTimeMin()) + kMin;

	//前回のプレイ
	m_lastPlayDateText = kLastPlayDateText + timeData->GetLastPlayDate();

	//クリア数に合わせてテキストを表示
	auto clearData = saveDataManager.GetClearData();
	int clearNum = static_cast<int>(clearData->IsClearStage1()) + 
		static_cast<int>(clearData->IsClearStage2()) + 
		static_cast<int>(clearData->IsClearStage3());
	std::wstring clearRate = kClearRate0;
	switch (clearNum)
	{
	case 0:
		clearRate = kClearRate0;
		break;
	case 1:
		clearRate = kClearRate1;
		break;
	case 2:
		clearRate = kClearRate2;
		break;
	case 3:
		clearRate = kClearRate3;
		break;
	default:
		break;
	}
	m_clearRateText = kClearRateText + clearRate;
}

SaveDataUI::~SaveDataUI()
{
}

void SaveDataUI::Update()
{
	if (m_isDraw)
	{
		//画面内に登場
		m_pos = Vector2::Lerp(m_pos, kNormalPos, kLerpRate);
	}
	else
	{
		//画面外
		m_pos = kOutPos;
	}
}

void SaveDataUI::Draw() const
{
	if (!m_isDraw)return;

	//背景
	DrawRotaGraphF(m_pos.x + kShadowOffsetX, m_pos.y + kShadowOffsetY, 1.0, 0.0, m_backShadowHandle, true);
	DrawRotaGraphF(m_pos.x, m_pos.y, 1.0, 0.0, m_backHandle, true);

	//セーブデータ
	DrawStringToHandle(m_pos.x + kSaveDataTextOffsetPosX, m_pos.y + kSaveDataTextOffsetPosY, kSaveDataText.c_str(), 0x00000, m_fontHandle);

	//名前
	DrawStringToHandle(m_pos.x + kSaveDataNameTextOffsetPosX, m_pos.y + kSaveDataNameTextOffsetPosY, kSaveDataNameText.c_str(), 0x00000, m_fontHandle);

	//任務進行度
	DrawStringToHandle(m_pos.x + kClearRateTextOffsetPosX, m_pos.y + kClearRateTextOffsetPosY, m_clearRateText.c_str(), 0x00000, m_fontHandle);

	//総プレイ時間
	DrawStringToHandle(m_pos.x + kPlayTimeTextOffsetPosX, m_pos.y + kPlayTimeTextOffsetPosY, m_totalPlayTimeText.c_str(), 0x00000, m_fontHandle);

	//前回のセーブ
	DrawStringToHandle(m_pos.x + kLastPlayDateTextOffsetPosX, m_pos.y + kLastPlayDateTextOffsetPosY, m_lastPlayDateText.c_str(), 0x00000, m_fontHandle);
}
