#pragma once
#include "CSVData.h"
#include "../StageIndex.h"
#include <memory>
#include <string>
class HighScoreData :
    public CSVData
{
public:
	HighScoreData();
	HighScoreData(std::shared_ptr<CSVData> data);
	~HighScoreData();

	void Reset();

	//ステージ番号
	StageIndex GetStageIndex()const { return m_stageIndex; };
	//タイム
	std::wstring GetTimeText()const { return m_timeText; };
	int GetTime()const { return m_time; };
	//ランク
	std::wstring GetRankText()const { return m_rankText; };

	//タイムセット
	void SetTimeText(std::wstring time);
	//ランクセット
	void SetRankText(std::wstring rank) { m_rankText = rank; };
private:
	//変換
	void Conversion() override;

	//総時間
	int GetAllTime()const;
private:
	//ステージ番号
	StageIndex m_stageIndex;
	//タイム
	std::wstring m_timeText;
	int m_time;
	//ランク
	std::wstring m_rankText;
};

