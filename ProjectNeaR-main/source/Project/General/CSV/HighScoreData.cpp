#include "HighScoreData.h"
#include "ClearSaveData.h"
#include "../SaveDataManager.h"

namespace
{
	constexpr int kDataNum = 3;
}

HighScoreData::HighScoreData():
	m_stageIndex(StageIndex::Stage1),
	m_timeText(L"ーー:ーー:ーー"),
	m_rankText(L"ー"),
	m_time(0)
{
}

HighScoreData::HighScoreData(std::shared_ptr<CSVData> data) :
	m_stageIndex(StageIndex::Stage1),
	m_timeText(L"ーー:ーー:ーー"),
	m_rankText(L"ー"),
	m_time(0)
{
	//データを取得
	this->m_data = data->GetData();
	Conversion();
}

HighScoreData::~HighScoreData()
{
}

void HighScoreData::Reset()
{
	m_timeText = L"ーー:ーー:ーー";
	m_rankText = L"ー";
	m_time = 0;
}
void HighScoreData::SetTimeText(std::wstring time)
{
	m_timeText = time;
	m_time = GetAllTime();
}

void HighScoreData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;

	int index = std::stoi(m_data[0]);
	switch (index)
	{
	case 1:
		m_stageIndex = StageIndex::Stage1;
		break;
	case 2:
		m_stageIndex = StageIndex::Stage2;
		break;
	case 3:
		m_stageIndex = StageIndex::Stage3;
		break;
	default:
		m_stageIndex = StageIndex::Stage1;
		break;
	}

	auto clearData = SaveDataManager::GetInstance().GetClearData();
	if (!clearData)return;
	
	//クリアしてないならreturn
	switch (m_stageIndex)
	{
	case StageIndex::Stage1:
		if (!clearData->IsClearStage1())return;
		break;
	case StageIndex::Stage2:
		if (!clearData->IsClearStage2())return;
		break;
	case StageIndex::Stage3:
		if (!clearData->IsClearStage3())return;
		break;
	default:
		return;
		break;
	}

	//タイム
	m_timeText = m_data[1];
	m_time = GetAllTime();

	//ランク
	m_rankText = m_data[2];
}

int HighScoreData::GetAllTime() const
{
	int minute = 0;
	int second = 0;
	int millisecond = 0;

	size_t pos1 = m_timeText.find(L':');
	size_t pos2 = m_timeText.find(L':', pos1 + 1);

	minute = std::stoi(m_timeText.substr(0, pos1));
	second = std::stoi(m_timeText.substr(pos1 + 1, pos2 - pos1 - 1));
	millisecond = std::stoi(m_timeText.substr(pos2 + 1));
	return minute * 3600 + second * 60 + millisecond;
}
