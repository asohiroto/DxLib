#pragma once
#include "CSVData.h"
#include <memory>
#include <string>
#include "../Math/MyMath.h"
class ResultRankData :
	public CSVData
{
public:
	ResultRankData();
	ResultRankData(std::shared_ptr<CSVData> data);
	~ResultRankData();
	
	std::wstring GetRank()const { return m_rank; };
	int GetMin()const { return m_min; };
	int GetSec()const { return m_sec; };
private:
	//•ÏŠ·
	void Conversion() override;
	//ƒ‰ƒ“ƒN
	std::wstring m_rank;
	//•ª
	int m_min;
	//•b
	int m_sec;
};

