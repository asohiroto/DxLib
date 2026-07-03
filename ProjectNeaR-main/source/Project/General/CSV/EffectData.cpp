#include "EffectData.h"

namespace
{
	constexpr int kDataNum = 2;
}

EffectData::EffectData():
	m_name(),
	m_path()
{
}

EffectData::EffectData(std::shared_ptr<CSVData> data):
	m_name(),
	m_path()
{
	//データを取得
	this->m_data = data->GetData();
	//変換
	Conversion();
}

EffectData::~EffectData()
{
}

void EffectData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;
	//名前
	m_name = m_data[0];
	//攻撃のキー
	m_path = m_data[1];
}
