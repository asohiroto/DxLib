#include "PodData.h"

namespace
{
	constexpr int kDataNum = 5;
}

PodData::PodData():
	m_scale(Vector3::Zero()),
	m_modelPath(L""),
	m_at(0)
{
}

PodData::PodData(std::shared_ptr<CSVData> data) :
	m_scale(Vector3::Zero()),
	m_modelPath(L""),
	m_at(0)
{
	//データを取得
	this->m_data = data->GetData();
	Conversion();
}

PodData::~PodData()
{
}

void PodData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;

	//大きさ
	m_scale.x = std::stof(m_data[0]);
	m_scale.y = std::stof(m_data[1]);
	m_scale.z = std::stof(m_data[2]);

	//モデルのパス
	m_modelPath = m_data[3];

	//攻撃タイプ
	m_at = stoi(m_data[4]);
}
