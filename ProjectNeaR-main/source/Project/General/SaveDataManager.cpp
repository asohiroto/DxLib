#include "SaveDataManager.h"
#include "CSV/CSVDataLoader.h"
#include "CSV/CSVDataSaver.h"
#include "CSV/ClearSaveData.h"
#include "CSV/OptionData.h"
#include "CSV/HighScoreData.h"
#include "CSV/TimeSaveData.h"
#include "../Main/Application.h"

namespace
{
	//パス
	const std::wstring kClearSaveDataPath = L"Save/ClearSaveData";
	const std::wstring kOptionSaveDataPath = L"Save/OptionSaveData";
	const std::wstring kHighScoreSaveDataPath = L"Save/HighScoreSaveData";
	const std::wstring kTimeSaveDataPath = L"Save/TimeSaveData";
}

void SaveDataManager::LoadSaveData()
{
	auto& csvLoader = CSVDataLoader::GetInstance();

	//クリアデータのロード
	auto clearData = csvLoader.LoadCSV(kClearSaveDataPath.c_str()).front();
	m_clearData = std::make_shared<ClearSaveData>(clearData);

	//オプションデータのロード
	auto optionData = csvLoader.LoadCSV(kOptionSaveDataPath.c_str()).front();
	m_optionData = std::make_shared<OptionData>(optionData);

	//日付と総プレイ時間データのロード
	auto timeSaveData = csvLoader.LoadCSV(kTimeSaveDataPath.c_str()).front();
	m_timeSaveData = std::make_shared<TimeSaveData>(timeSaveData);

	//スコアデータのロード
	for (auto& data : csvLoader.LoadCSV(kHighScoreSaveDataPath.c_str()))
	{
		m_highScoreDatas.emplace_back(std::make_shared<HighScoreData>(data));
	}
}

void SaveDataManager::SaveNewData()
{
	//全てのデータをリセット
	m_clearData->Reset();
	m_timeSaveData->Reset();
	for (auto& data : m_highScoreDatas)
	{
		data->Reset();
	}

	//セーブ
	SaveAllData();
}

void SaveDataManager::SaveAllData()
{
	auto& saver = CSVDataSaver::GetInstance();
	//セーブ
	saver.SaveClearData();
	saver.SaveHighScoreData();
	saver.SaveOption();
	m_timeSaveData->SetLastPlayDate(m_timeSaveData->GetNowTimeAndDate());	//セーブした日付更新
	saver.SaveTimeData();
}

void SaveDataManager::SaveOptionData()
{
	//設定をセーブする
	CSVDataSaver::GetInstance().SaveOption();
}

void SaveDataManager::AddPlayTime()
{
	m_timeSaveData->AddPlayTime();
}

std::shared_ptr<HighScoreData> SaveDataManager::GetHighScoreData(StageIndex index) const
{
	//一致したものを返す
	for (auto& data : m_highScoreDatas)
	{
		if (data->GetStageIndex() == index)
		{
			return data;
		}
	}

	return m_highScoreDatas.front();
}
