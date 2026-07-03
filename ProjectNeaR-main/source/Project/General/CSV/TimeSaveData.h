#pragma once
#include "CSVData.h"
#include <memory>

class TimeSaveData :
    public CSVData
{
public:
	TimeSaveData();
	TimeSaveData(std::shared_ptr<CSVData> data);
	~TimeSaveData();

	void Reset();
	
	//現在の日付と時間
	std::wstring GetLastPlayDate()const { return m_lastPlayDate; };
	void SetLastPlayDate(std::wstring lastDate) { m_lastPlayDate = lastDate; };
	//プレイ時間
	unsigned int GetTotalPlayTime()const { return m_totalPlayTime; };
	void SetTotalPlayTime(int time) { m_totalPlayTime = time; };

	//現在の日付と時間
	std::wstring GetNowTimeAndDate();
	//プレイ時間の加算
	void AddPlayTime() { ++m_totalPlayTime; };

	//時
	unsigned int GetPlayTimeHour()const;
	//分
	unsigned int GetPlayTimeMin()const;
private:
	//変換
	void Conversion() override;
private:
	//プレイ時間
	unsigned int m_totalPlayTime;

	//日付と時間
	std::wstring m_lastPlayDate;
};

