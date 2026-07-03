#include "TextData.h"

namespace
{
	constexpr int kDataNum = 6;
}

TextData::TextData() :
	m_id(L""),
	m_text(L""),
	m_speaker(L""),
	m_nextID(L""),
	m_group(L""),
	m_imagePath(L"")
{
}

TextData::TextData(std::shared_ptr<CSVData> data):
	m_id(L""),
	m_text(L""),
	m_speaker(L""),
	m_nextID(L""),
	m_group(L""),
	m_imagePath(L"")
{
	//データを取得
	this->m_data = data->GetData();
	Conversion();
}

TextData::~TextData()
{
}

void TextData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;

	//ID
	m_id = m_data[0];
	//テキスト
	m_text = m_data[1];
	//話してる人
	m_speaker = m_data[2];
	//次のID
	m_nextID = m_data[3];
	//グループ
	m_group = m_data[4];
	//画像パス
	m_imagePath = m_data[5];
}
