#include "MissionBoardUIData.h"

namespace
{
	constexpr int kDataNum = 3;
}

MissionBoardUIData::MissionBoardUIData() :
	m_path(L""),
	m_text(L""),
	m_title(L"")
{
}

MissionBoardUIData::MissionBoardUIData(std::shared_ptr<CSVData> data) :
	m_path(L""),
	m_text(L""),
	m_title(L"")
{
	//データを取得
	this->m_data = data->GetData();
	//変換
	Conversion();
}

MissionBoardUIData::~MissionBoardUIData()
{
}

void MissionBoardUIData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;

	//パス
	m_path = m_data[0];
	//テキスト
	m_text = m_data[1];
	//タイトル
	m_title = m_data[2];
}
