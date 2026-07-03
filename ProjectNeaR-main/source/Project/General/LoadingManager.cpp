#include "LoadingManager.h"
#include "Game.h"
#include "Math/MyMath.h"
#include "Sound/SoundManager.h"
#include "AssetManager.h"
#include "CSV/CSVDataLoader.h"
#include "CSV/CSVData.h"
#include <DxLib.h>

namespace
{
	//最低ロードフレーム
	constexpr int kMinLoadFrame = 180;
	//ロード終了後すぐに切り替わらないようにするためのフレーム数
	constexpr int kAfterLoadFrame = 30;

	//ギアの座標
	constexpr float kGear1PosX = Game::kScreenWidth;
	constexpr float kGear1PosY = 50.0f;
	constexpr float kGear2PosX = 0.0f;
	constexpr float kGear2PosY = Game::kScreenHeight;

	//Loading座標
	constexpr float kLoadingTextPosX = 50.0f;
	constexpr float kLoadingTextPosY = 50.0f;
	//.座標
	constexpr float kLoadingPointTextPosX = kLoadingTextPosX + 280.0f;
	constexpr float kLoadingPointTextOffsetPosX = 30.0f;
	constexpr float kLoadingPointTextPosY = kLoadingTextPosY;

	//ロードテキスト座標
	constexpr float kTextPosX = 200.0f;
	constexpr float kTextPosY = 160.0f;
	constexpr float kTextPosOffsetY = 28.0f;

	//.を表示する間隔
	constexpr int kResetPointFrame = 40;
	constexpr int kPointFrame = 10;

	//パス
	const std::wstring kBackPath = L"Data/UI/Loading/LoadingBack.png";
	const std::wstring kGearPath = L"Data/UI/Loading/LoadGear.png";
	const std::wstring kLoadingTextPath = L"Data/UI/Loading/LoadingText.png";
	const std::wstring kLoadingTextPointPath = L"Data/UI/Loading/LoadingTextPoint.png";

	//フォント
	const std::wstring kNotoName = L"Noto Sans JP Light";
	constexpr int kFontSize = 20;

	//テキストパス
	const std::wstring kTextCSVPath = L"Loading/LoadingText";
}

void LoadingManager::Init()
{
	m_loadCountFrame = 0;
	m_afterLoadFrame = 0;
	m_loadingRate = 0;
	m_gearRotaAngle = 0;
	m_loadingPointCountFrame = 0;

	auto& assetManager = AssetManager::GetInstance();
	//画像
	m_backHandle = LoadGraph(kBackPath.c_str());
	m_loadingGearHandle = LoadGraph(kGearPath.c_str());
	m_loadingTextHandle = LoadGraph(kLoadingTextPath.c_str());
	m_loadingTextPointHandle = LoadGraph(kLoadingTextPointPath.c_str());

	//フォント
	m_fontHandle = CreateFontToHandle(kNotoName.c_str(), kFontSize, 5, DX_FONTTYPE_ANTIALIASING);

	//テキスト
	auto& csvLoader = CSVDataLoader::GetInstance();
	auto textDatas = csvLoader.LoadCSV(kTextCSVPath.c_str());
	for (auto& text : textDatas)
	{
		m_loadingTexts.emplace_back(text->GetData().front());
	}
}


void LoadingManager::Update()
{
	if (m_isLoading)
	{
		auto& soundManager = SoundManager::GetInstance();
		//全ての音を一時停止
		soundManager.AllStop();

		//回転
		++m_gearRotaAngle;

		//Loadingの.の更新
		++m_loadingPointCountFrame;
		if (m_loadingPointCountFrame > kResetPointFrame)m_loadingPointCountFrame = 0;

		//ロード数取得
		int loadNum = GetASyncLoadNum();

		//最低ロード時間
		if (m_loadCountFrame > 0)
		{
			--m_loadCountFrame;
		}

		//もしもロードが完了していて、最低ロード時間も過ぎていたら
		if (loadNum <= 0 && m_loadCountFrame <= 0 && !AssetManager::GetInstance().IsLoading())
		{
			--m_afterLoadFrame;
			if (m_afterLoadFrame <= 0)
			{
				//ロード終了
				m_isLoading = false;
				//全ての音を再生
				soundManager.AllPlay();
			}
		}

		float rate = 1.0f - (static_cast<float>(GetASyncLoadNum() + m_loadCountFrame) / m_totalLoadNum);
		rate *= 100.0f;
		m_loadingRate = static_cast<int>(rate);
	}
}
void LoadingManager::Draw()
{
	if (m_isLoading && m_totalLoadNum > 0)
	{
		//背景
		if (m_backHandle > -1)DrawGraph(0, 0, m_backHandle, true);

		//ギア
		float gearAngle = m_gearRotaAngle * MyMath::DEG_2_RAD;
		if (m_loadingGearHandle > -1)DrawRotaGraphF(kGear1PosX, kGear1PosY, 1.0f, gearAngle, m_loadingGearHandle,true);
		if (m_loadingGearHandle > -1)DrawRotaGraphF(kGear2PosX, kGear2PosY, 1.0f, gearAngle, m_loadingGearHandle,true);

		//Loading
		if (m_loadingTextHandle > -1)DrawGraphF(kLoadingTextPosX, kLoadingTextPosY, m_loadingTextHandle, true);


		if (m_loadingTextPointHandle > -1)
		{
			//.1
			if (m_loadingPointCountFrame > kPointFrame)
			{
				DrawGraphF(kLoadingPointTextPosX + kLoadingPointTextOffsetPosX, kLoadingTextPosY, m_loadingTextPointHandle, true);
			}
			//.2
			if (m_loadingPointCountFrame > kPointFrame * 2)
			{
				DrawGraphF(kLoadingPointTextPosX + (kLoadingPointTextOffsetPosX * 2.0f), kLoadingTextPosY, m_loadingTextPointHandle, true);
			}
			//.3
			if (m_loadingPointCountFrame > kPointFrame * 3)
			{
				DrawGraphF(kLoadingPointTextPosX + (kLoadingPointTextOffsetPosX * 3.0f), kLoadingTextPosY, m_loadingTextPointHandle, true);
			}
		}

		//テキスト
		if (m_loadingTexts.empty())return;
		if (m_fontHandle == -1)return;

		//ある一定パーセントごとに表示
		int rateOffset = 100 / m_loadingTexts.size();
		if (rateOffset <= 0)return;

		for (int i = 0; i < m_loadingTexts.size(); ++i)
		{
			DrawStringFToHandle(kTextPosX, kTextPosY + kTextPosOffsetY * i, m_loadingTexts[i].c_str(), 0xffffff, m_fontHandle);
			if (rateOffset * i > m_loadingRate)break;
		}
	}
}
void LoadingManager::End()
{
	if(m_backHandle >= 0)DeleteGraph(m_backHandle);
	if (m_loadingGearHandle >= 0)DeleteGraph(m_loadingGearHandle);
	if (m_loadingTextHandle >= 0)DeleteGraph(m_loadingTextHandle);
	if (m_loadingTextPointHandle >= 0)DeleteGraph(m_loadingTextPointHandle);
	if (m_fontHandle >= 0)DeleteFontToHandle(m_fontHandle);
}

void LoadingManager::StartLoading()
{
	//非同期ロード開始
	SetUseASyncLoadFlag(true);

	//ロード開始
	m_isLoading = true;

	//ロード時間カウントリセット
	m_loadCountFrame = kMinLoadFrame;

	//ロード終了後すぐに切り替わらないようにするためのフレーム数リセット
	m_afterLoadFrame = kAfterLoadFrame;

	//音停止
	SoundManager::GetInstance().AllStop();
}
void LoadingManager::EndLoading()
{
	//現在のロード数取得
	m_totalLoadNum = GetASyncLoadNum() + m_loadCountFrame;

	//非同期ロード終了
	SetUseASyncLoadFlag(false);
}