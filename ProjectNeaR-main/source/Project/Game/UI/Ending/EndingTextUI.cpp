#include "EndingTextUI.h"
#include "../../../General/CSV/EndingTextUIData.h"
#include "../../../General/CSV/CSVDataLoader.h"
#include "../../../General/Game.h"
#include "../../../General/AssetManager.h"
#include <algorithm>
#include <string>

namespace
{
	//パス
	const std::wstring kEndingTextUIPath = L"Ending/EndingText";
	const std::wstring kTitleUIPath = L"Title/Title";
	//テキスト開始Y座標
	constexpr float kTextStartPosY = Game::kScreenCenterY + 200.0f;
	//テキストX座標
	constexpr float kTextPosX = Game::kScreenCenterX - 500.0f;

	//画面外
	constexpr float kOffScreenMinY = -100.0f;
	constexpr float kOffScreenMaxY = Game::kScreenHeight + 100.0f;

	//スクロール速度
	constexpr float kScrollSpeed = 1.5f;

	//黒背景
	constexpr float kBlackAlpha = 150.0f;
	constexpr float kBlackAlphaSpeed = 2.0f;

	//タイトル透明度速度
	constexpr float kTitleAlphaSpeed = 2.0f;

}

EndingTextUI::EndingTextUI():
	m_endingDatas(),
	m_fontDatas(),
	m_scrollOffsetYs(),
	m_scrollY(kTextStartPosY),
	m_blackAlpha(kBlackAlpha),
	m_titleHandle(-1),
	m_titleAlpha(0.0f)
{
	auto& assetManager = AssetManager::GetInstance();

	float scrollOffsetY = 0;
	//データのロード
	for (auto& data : CSVDataLoader::GetInstance().LoadCSV(kEndingTextUIPath.c_str()))
	{
		auto endingData = std::make_shared<EndingTextUIData>(data);
		//テキストデータを保存
		m_endingDatas.push_back(endingData);

		//フォント
		auto font = endingData->GetFont();
		//テキスト
		auto text = endingData->GetText();

		//フォントハンドルを取得して保存
		m_fontDatas.push_back(assetManager.GetFontHandle(font));
		//スクロールオフセットを保存
		scrollOffsetY += static_cast<int>(font.size) * 2.0f;
		m_scrollOffsetYs.push_back(scrollOffsetY);
	}

	//タイトルUI
	m_titleHandle = assetManager.GetImageHandle(kTitleUIPath.c_str());
}

EndingTextUI::~EndingTextUI()
{
}

void EndingTextUI::Update()
{
	bool isAllOffScreen = true;
	for(auto& offsetPosY : m_scrollOffsetYs)
	{
		float posY = m_scrollY + offsetPosY;
		//画面内にあるなら
		if (posY > kOffScreenMinY)
		{
			isAllOffScreen = false;
			break;
		}
	}
	//すべて画面外なら
	if (isAllOffScreen)
	{
		//フェード
		m_blackAlpha += kBlackAlphaSpeed;
		m_blackAlpha = MathSub::ClampFloat(m_blackAlpha, 0.0f, 255.0f);

		//もしも最大値なら
		if (m_blackAlpha >= 255.0f)m_titleAlpha += kTitleAlphaSpeed;
	}

	//スクロール
	m_scrollY -= kScrollSpeed;
}

void EndingTextUI::Draw() const
{
	//少し暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_blackAlpha);
	//背景描画
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//順番にテキストを描画していく
	for (int i = 0; i < m_endingDatas.size() || i < m_fontDatas.size() || i < m_scrollOffsetYs.size(); ++i)
	{
		//テキスト
		auto text = m_endingDatas[i]->GetText();
		//描画座標
		float posY = m_scrollY + m_scrollOffsetYs[i];
		
		if(posY < kOffScreenMinY || posY > kOffScreenMaxY)
		{
			//画面外なら描画しない
			continue;
		}

		//描画
		DrawStringFToHandle(kTextPosX , posY, m_endingDatas[i]->GetText().c_str(), 0xffffff, m_fontDatas[i]);

	}

	//タイトルUI描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_titleAlpha);
	DrawRotaGraphF(Game::kScreenCenterX, Game::kScreenCenterY, 1.0f, 0.0f, m_titleHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
