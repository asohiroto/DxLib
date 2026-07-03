#include "CharaStatusData.h"

namespace
{
	constexpr int kDataNum = 12;
}

CharaStatusData::CharaStatusData() :
	m_id(0),
	m_jp(0.0f),
	m_hp(0),
	m_at(0),
	m_df(0),
	m_ms(0.0f),
	m_ar(CharaStatus::Armor::Light),
	m_searchAngle(0.0f),
	m_searchRange(0.0f),
	m_meleeAttackRange(0.0f),
	m_groupTag(L"")
{
}

CharaStatusData::CharaStatusData(std::shared_ptr<CSVData> data) :
	m_id(0),
	m_jp(0.0f),
	m_hp(0),
	m_at(0),
	m_df(0),
	m_ms(0.0f),
	m_ar(CharaStatus::Armor::Light),
	m_searchAngle(0.0f),
	m_searchRange(0.0f),
	m_meleeAttackRange(0.0f),
	m_groupTag(L"")
{
	//データを取得
	this->m_data = data->GetData();
	//変換
	Conversion();
}

CharaStatusData::~CharaStatusData()
{
}

void CharaStatusData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;
	//名前はスキップ
	//1スタート
	//ID
	m_id = stoi(m_data[1]);
	//体力
	m_hp = stoi(m_data[2]);
	//攻撃力
	m_at = stoi(m_data[3]);
	//防御力
	m_df = stoi(m_data[4]);
	//移動速度
	m_ms = stof(m_data[5]);
	//ジャンプ力
	m_jp = stof(m_data[6]);
	//アーマー
	m_ar = static_cast<CharaStatus::Armor>(stoi(m_data[7]));
	//索敵半径
	m_searchRange = stof(m_data[8]);
	//索敵アングル
	m_searchAngle = stof(m_data[9]) * MyMath::DEG_2_RAD;
	//近接攻撃距離
	m_meleeAttackRange = stof(m_data[10]);
	//グループタグ
	m_groupTag = m_data[11];
}