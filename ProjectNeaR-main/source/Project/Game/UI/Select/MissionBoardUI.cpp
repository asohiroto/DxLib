#include "MissionBoardUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Game.h"
#include "../../../General/StringUtil.h"
#include "../../../General/CSV/MissionBoardUIData.h"
#include "../../../General/SaveDataManager.h"
#include "../../../General/CSV/ClearSaveData.h"
#include "../../../General/CSV/HighScoreData.h"

namespace
{
	//パス
	const std::wstring kMissionBoardPath = L"Select/MissionBoard";
	const std::wstring kMissionBoardShadowPath = L"Select/MissionBoard_Shadow";
	const std::wstring kMissionBoardClearPath = L"Select/MissionBoardClear";
	const std::wstring kScoreBoardPath = L"Select/ScoreBoard";
	const std::wstring kScoreBoardShadowPath = L"Select/ScoreBoard_Shadow";

	//座標
	constexpr float kMissionBoardPosX = Game::kScreenCenterX + 200.0f;
	constexpr float kMissionBoardPosY = Game::kScreenCenterY;
	constexpr float kScoreBoardPosX = kMissionBoardPosX + 300.0f;
	constexpr float kScoreBoardPosY = kMissionBoardPosY - 100.0f;
	constexpr float kTimePosX = kScoreBoardPosX;
	constexpr float kTimePosY = kScoreBoardPosY - 35.0f;
	constexpr float kMinClearTimePosX = kScoreBoardPosX - 85.0f;
	constexpr float kMinClearTimePosY = kScoreBoardPosY - 70.0f;
	constexpr float kRankPosX = kScoreBoardPosX;
	constexpr float kRankPosY = kScoreBoardPosY + 30.0f;
	constexpr float kRankTextPosX = kScoreBoardPosX - 24.0f;
	constexpr float kRankTextPosY = kScoreBoardPosY;

	//影
	constexpr float kMissionBoardShadowOffsetX = 10.0f;
	constexpr float kMissionBoardShadowOffsetY = 10.0f;
	constexpr float kScoreBoardPosShadowOffsetX = 10.0f;
	constexpr float kScoreBoardPosShadowOffsetY = 10.0f;

	//テキスト座標
	constexpr float kMissionBoardTextPosX = kMissionBoardPosX - 250.0f;
	constexpr float kMissionBoardTextPosY = kMissionBoardPosY + 20.0f;

	//タイトル座標
	constexpr float kMissionBoardTitlePosX = kMissionBoardPosX;
	constexpr float kMissionBoardTitlePosY = kMissionBoardPosY - 268.0f;

	//最短クリア記録
	const std::wstring kMinClearTimeText = L"最短クリア記録";
	//評価
	const std::wstring kRankText = L"評価";
}

MissionBoardUI::MissionBoardUI(int size, std::vector<std::shared_ptr<MissionBoardUIData>> datas,bool isTutorial):
	m_index(0),
	m_backHandle(-1),
	m_backShadowHandle(-1),
	m_textFontHandle(-1),
	m_titleFontHandle(-1),
	m_timeFontHandle(-1),
	m_scoreFontHandle(-1),
	m_clearMarkHandle(-1),
	m_scoreBoardHandle(-1),
	m_scoreBoardShadowHandle(-1),
	m_rankFontHandle(-1),
	m_datas(datas),
	m_isTutorial(isTutorial)
{
	auto& assetManager = AssetManager::GetInstance();
	//ハンドルロード
	for(int i = 0; i < size; ++i)
	{
		m_handles[i] = assetManager.GetImageHandle(kMissionBoardPath.c_str() + m_datas[i]->GetPath());
	}
	m_backHandle = assetManager.GetImageHandle(kMissionBoardPath.c_str());
	m_backShadowHandle = assetManager.GetImageHandle(kMissionBoardShadowPath.c_str());
	m_clearMarkHandle = assetManager.GetImageHandle(kMissionBoardClearPath.c_str());

	//フォント
	m_textFontHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::NotoSansJP, AssetManager::FontSize::Size20));
	m_titleFontHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::NotoSansJP, AssetManager::FontSize::Size32));

	//チュートリアルじゃないなら
	if (!m_isTutorial)
	{
		//クリアした記録を表示するのでロード
		m_scoreBoardHandle = assetManager.GetImageHandle(kScoreBoardPath.c_str());
		m_scoreBoardShadowHandle = assetManager.GetImageHandle(kScoreBoardShadowPath.c_str());
		m_timeFontHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::NotoSansJP, AssetManager::FontSize::Size24));
		m_rankFontHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::NotoSansJP, AssetManager::FontSize::Size36));
		m_scoreFontHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::NotoSansJP, AssetManager::FontSize::Size24));
	}
}

MissionBoardUI::~MissionBoardUI()
{
}

void MissionBoardUI::Update()
{
}

void MissionBoardUI::Draw() const
{
	if (!m_isDraw)return;
	
	//影描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	DrawRotaGraph(kMissionBoardPosX + kMissionBoardShadowOffsetX, kMissionBoardPosY + kMissionBoardShadowOffsetY, 1.0f, 0.0f, m_backShadowHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//背景描画
	DrawRotaGraph(kMissionBoardPosX, kMissionBoardPosY, 1.0f, 0.0f, m_backHandle, true);

	//ミッションボード描画
	DrawRotaGraph(kMissionBoardPosX, kMissionBoardPosY, 1.0f, 0.0f, m_handles.at(m_index), true);

	//テキスト
	auto text = m_datas[m_index]->GetText();
	DrawStringFToHandle(kMissionBoardTextPosX, kMissionBoardTextPosY, text.c_str(), 0x000000, m_textFontHandle);

	//タイトル
	auto title = m_datas[m_index]->GetTitle();
	//中央ぞろえ
	float titleWidth = StringUtil::GetTextWidth(title, m_titleFontHandle);
	if (titleWidth > 0.0f)
	{
		titleWidth *= 0.5f;
	}
	DrawStringFToHandle(kMissionBoardTitlePosX - titleWidth,kMissionBoardTitlePosY, title.c_str(), 0x000000, m_titleFontHandle);

	//クリアしたか
	auto clearData = SaveDataManager::GetInstance().GetClearData();

	bool isClear = false;
	switch (m_index)
	{
	case 0:
		if (m_isTutorial)isClear = clearData->IsClearTutorial1();
		else isClear = clearData->IsClearStage1();
		break;
	case 1:
		if (m_isTutorial)isClear = clearData->IsClearTutorial2();
		else isClear = clearData->IsClearStage2();
		break;
	case 2:
		if (m_isTutorial)isClear = clearData->IsClearTutorial3();
		else isClear = clearData->IsClearStage3();
		break;
	default:
		break;
	}
	if (isClear)
	{
		DrawRotaGraph(kMissionBoardPosX, kMissionBoardPosY, 1.0f, 0.0f, m_clearMarkHandle, true);
	}

	//影描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	DrawRotaGraph(kScoreBoardPosX + kScoreBoardPosShadowOffsetX, kScoreBoardPosY + kScoreBoardPosShadowOffsetY, 1.0f, 0.0f, m_scoreBoardShadowHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//チュートリアルなら描画しない
	if (m_isTutorial)return;

	//スコアデータ
	auto scoreData = SaveDataManager::GetInstance().GetHighScoreData(static_cast<StageIndex>(m_index));
	std::wstring timeText = scoreData->GetTimeText();
	std::wstring rankText = scoreData->GetRankText();

	//ハイスコアを描画
	DrawRotaGraph(kScoreBoardPosX, kScoreBoardPosY, 1.0f, 0.0f, m_scoreBoardHandle, true);
	
	//タイマー
	float timeWidth = StringUtil::GetTextWidth(timeText, m_timeFontHandle);
	if (timeWidth > 0.0f)
	{
		timeWidth *= 0.5f;
	}
	DrawStringFToHandle(kMinClearTimePosX, kMinClearTimePosY, kMinClearTimeText.c_str(), 0xaa2222, m_scoreFontHandle);
	DrawStringFToHandle(kTimePosX - timeWidth, kTimePosY, timeText.c_str(), 0x000000, m_timeFontHandle);

	//ランク
	float rankWidth = StringUtil::GetTextWidth(rankText, m_rankFontHandle);
	if (rankWidth > 0.0f)
	{
		rankWidth *= 0.5f;
	}
	DrawStringFToHandle(kRankTextPosX, kRankTextPosY, kRankText.c_str(), 0xaa2222, m_scoreFontHandle);
	DrawStringFToHandle(kRankPosX - rankWidth, kRankPosY, rankText.c_str(), 0x000000, m_rankFontHandle);

}
