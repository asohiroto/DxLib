#include "TutorialClearData.h"

namespace
{
	constexpr int kDataNum = 3;
}

TutorialClearData::TutorialClearData():
	m_clearRequirement(ClearRequirement::AllDeadEnemy)
{
}

TutorialClearData::TutorialClearData(std::shared_ptr<CSVData> data):
	m_clearRequirement(ClearRequirement::AllDeadEnemy)
{
	//データを取得
	this->m_data = data->GetData();
	Conversion();
}

TutorialClearData::~TutorialClearData()
{
}

void TutorialClearData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;

	//チュートリアル名
	m_tutorialName = m_data[0];
	//クリア条件
	m_clearRequirement = static_cast<ClearRequirement>(stoi(m_data[1]));
	//クリア条件テキスト
	m_clearRequirementText = m_data[2];
}
