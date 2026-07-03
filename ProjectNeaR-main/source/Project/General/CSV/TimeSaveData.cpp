#include "TimeSaveData.h"
#include "../../Main/Application.h"
#include "../Math/MyMath.h"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace
{
	constexpr int kDataNum = 2;

	//FPS
	constexpr unsigned int kFPS = 60;
	//時
	constexpr unsigned int kHour = 3600;
	//分
	constexpr unsigned int kMin = 60;
}

TimeSaveData::TimeSaveData() :
	m_totalPlayTime(0),
	m_lastPlayDate(L"NO DATA")
{
}

TimeSaveData::TimeSaveData(std::shared_ptr<CSVData> data) :
	m_totalPlayTime(0),
	m_lastPlayDate(L"NO DATA")
{
	//データを取得
	this->m_data = data->GetData();
	//変換
	Conversion();
}

TimeSaveData::~TimeSaveData()
{
}
void TimeSaveData::Reset()
{
	m_totalPlayTime = 0;
	m_lastPlayDate = L"NO DATA";
}

std::wstring TimeSaveData::GetNowTimeAndDate()
{
	//現在時刻を取得
	std::time_t t = std::time(nullptr);
	std::tm tm;
	localtime_s(&tm, &t);

	//wstringに変換(例: 2026-01-9 12:34:56)
	std::wostringstream woss;
	woss << std::put_time(&tm, L"%Y-%m-%d %H:%M:%S");
	std::wstring nowTime = woss.str();
	return nowTime;
}

unsigned int TimeSaveData::GetPlayTimeHour() const
{
	if (m_totalPlayTime > 0)
	{
		return MathSub::ClampInt((m_totalPlayTime / kFPS) / kHour, 0, 99);
	}

	return 0;
}

unsigned int TimeSaveData::GetPlayTimeMin() const
{
	if (m_totalPlayTime > 0)
	{
		return MathSub::ClampInt(((m_totalPlayTime / kFPS) % kHour) / kMin, 0, 59);
	}

	return 0;
}

void TimeSaveData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;
	
	//総プレイ時間
	m_totalPlayTime = std::stoi(m_data[0]);

	//最後のプレイした日付と時間
	m_lastPlayDate = m_data[1];
}
