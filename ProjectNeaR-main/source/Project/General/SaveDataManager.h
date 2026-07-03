#pragma once
#include <memory>
#include <vector>
#include "StageIndex.h"
class OptionData;
class ClearSaveData;
class HighScoreData;
class TimeSaveData;
class SaveDataManager final
{
private:
	//シングルトンの準備
	SaveDataManager() = default;
	~SaveDataManager() = default;
	//コピー禁止
	SaveDataManager(const SaveDataManager&) = delete;
	SaveDataManager& operator = (const SaveDataManager&) = delete;
	//ムーブ禁止
	SaveDataManager(SaveDataManager&&) = delete;
	SaveDataManager& operator = (SaveDataManager&&) = delete;
public:
	//インスタンスを取得
	static SaveDataManager& GetInstance()
	{
		static SaveDataManager instance;
		return instance;
	}
	//セーブデータのロード
	void LoadSaveData();

	//セーブデータの新規作成
	void SaveNewData();

	//セーブ
	void SaveAllData();

	//オプションのセーブ
	void SaveOptionData();

	//プレイ時間の加算
	void AddPlayTime();
	
	//クリアデータ
	std::shared_ptr<ClearSaveData> GetClearData()const { return m_clearData; };
	//オプションデータ
	std::shared_ptr<OptionData> GetOptionData()const { return m_optionData; };
	//日付と総プレイ時間データ
	std::shared_ptr<TimeSaveData> GetTimeSaveData()const { return m_timeSaveData; };
	//スコアデータ
	std::shared_ptr<HighScoreData> GetHighScoreData(StageIndex index)const;
private:
	//クリアデータ
	std::shared_ptr<ClearSaveData> m_clearData;
	//オプションデータ
	std::shared_ptr<OptionData> m_optionData;
	//日付と総プレイ時間データ
	std::shared_ptr<TimeSaveData> m_timeSaveData;
	//スコアデータ
	std::vector<std::shared_ptr<HighScoreData>> m_highScoreDatas;
};

