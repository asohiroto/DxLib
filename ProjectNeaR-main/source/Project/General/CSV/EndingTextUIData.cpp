#include "EndingTextUIData.h"

namespace
{
	constexpr int kDataNum = 2;
}

EndingTextUIData::EndingTextUIData() :
	m_text(L""),
	m_font()
{
}

EndingTextUIData::EndingTextUIData(std::shared_ptr<CSVData> data) :
	m_text(L""),
	m_font()
{
	//データを取得
	this->m_data = data->GetData();
	//変換
	Conversion();
}

EndingTextUIData::~EndingTextUIData()
{
}

void EndingTextUIData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;
	//テキスト
	m_text = m_data[0];
	//フォント
	m_font.type = AssetManager::FontType::NotoSansJP;
	m_font.size = static_cast<AssetManager::FontSize>(std::stoi(m_data[1]));
}
