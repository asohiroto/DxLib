#include "ResultRankData.h"

namespace 
{
	constexpr int kDataNum = 3;
}

ResultRankData::ResultRankData() :
	m_rank(L""),
	m_min(0),
	m_sec(0)
{
}

ResultRankData::ResultRankData(std::shared_ptr<CSVData> data) :
	m_rank(L""),
	m_min(0),
	m_sec(0)
{
	//データを取得
	this->m_data = data->GetData();
	Conversion();
}

ResultRankData::~ResultRankData()
{
}

void ResultRankData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;

	//ランク
	m_rank = m_data[0];

	//分と秒
	m_min = stoi(m_data[1]);
	m_sec = stoi(m_data[2]);
}
