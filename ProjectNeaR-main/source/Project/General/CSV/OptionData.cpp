#include "OptionData.h"

namespace
{
	constexpr int kDataNum = 4;
}

OptionData::OptionData():
	m_bgmLv(0),
	m_seLv(0),
	m_voiceLv(0),
	m_isWindow(false)
{
}

OptionData::OptionData(std::shared_ptr<CSVData> data):
	m_bgmLv(0),
	m_seLv(0),
	m_voiceLv(0),
	m_isWindow(false)
{
	//データを取得
	this->m_data = data->GetData();
	Conversion();
}

OptionData::~OptionData()
{
}

void OptionData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;

	//音量
	m_bgmLv = std::stoi(m_data[0]);
	m_seLv = std::stoi(m_data[1]);
	m_voiceLv = std::stoi(m_data[2]);
	//スクリーンモード
	int temp = std::stoi(m_data[3]);
	m_isWindow = temp != 0;
}
