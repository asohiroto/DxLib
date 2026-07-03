#include "ClearSaveData.h"

ClearSaveData::ClearSaveData():
	m_isClearTutorial1(false),
	m_isClearTutorial2(false),
	m_isClearTutorial3(false),
	m_isClearStage1(false),
	m_isClearStage2(false),
	m_isClearStage3(false)
{
}

ClearSaveData::ClearSaveData(std::shared_ptr<CSVData> data):
	m_isClearTutorial1(false),
	m_isClearTutorial2(false),
	m_isClearTutorial3(false),
	m_isClearStage1(false),
	m_isClearStage2(false),
	m_isClearStage3(false)
{
	//ƒf[ƒ^‚ðŽæ“¾
	this->m_data = data->GetData();
	Conversion();
}

ClearSaveData::~ClearSaveData()
{
}

void ClearSaveData::Reset()
{
	m_isClearTutorial1	= false;
	m_isClearTutorial2	= false;
	m_isClearTutorial3	= false;
	m_isClearStage1		= false;
	m_isClearStage2		= false;
	m_isClearStage3		= false;
}

void ClearSaveData::Conversion()
{
	m_isClearTutorial1	= std::stoi(m_data[0]) != 0;
	m_isClearTutorial2	= std::stoi(m_data[1]) != 0;
	m_isClearTutorial3	= std::stoi(m_data[2]) != 0;
	m_isClearStage1		= std::stoi(m_data[3]) != 0;
	m_isClearStage2		= std::stoi(m_data[4]) != 0;
	m_isClearStage3		= std::stoi(m_data[5]) != 0;
}
