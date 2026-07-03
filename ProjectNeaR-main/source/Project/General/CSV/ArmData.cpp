#include "ArmData.h"

namespace
{
	constexpr int kDataNum = 7;
}

ArmData::ArmData() :
	m_name(L""),
	m_hp(0),
	m_df(0),
	m_ar(CharaStatus::Armor::Light), 
	m_radius(0.0f),
	m_startIndex(0),
	m_endIndex(0)
{
}

ArmData::ArmData(std::shared_ptr<CSVData> data) :
	m_name(L""),
	m_hp(0),
	m_df(0),
	m_ar(CharaStatus::Armor::Light),
	m_radius(0.0f),
	m_startIndex(0),
	m_endIndex(0)
{
	//データを取得
	this->m_data = data->GetData();
	//変換
	Conversion();
}

ArmData::~ArmData()
{
}

void ArmData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;
	//名前
	m_name = m_data[0];
	//体力
	m_hp = std::stoi(m_data[1]);
	//防御
	m_df = std::stoi(m_data[2]);
	//アーマー
	CharaStatus::Armor m_ar = static_cast<CharaStatus::Armor>(std::stoi(m_data[3]));
	//半径
	m_radius = std::stof(m_data[4]);
	//始点
	m_startIndex = std::stoi(m_data[5]);
	//終点
	m_endIndex = std::stoi(m_data[6]);
}
