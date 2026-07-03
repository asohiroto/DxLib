#pragma once
#include "CSVData.h"
#include <memory>
#include <string>
#include "../Math/MyMath.h"
#include "../StageIndex.h"
class TutorialClearData :
	public CSVData
{
public:
	TutorialClearData();
	TutorialClearData(std::shared_ptr<CSVData> data);
	~TutorialClearData();
	
	//クリア条件
	enum class ClearRequirement : int
	{
		AllDeadEnemy = 0,
		Goal = 1,
		JustAvoid = 2
	};

	//チュートリアル名
	std::wstring GetTutorialName()const { return m_tutorialName; };
	//クリア条件
	ClearRequirement GetClearRequirement()const { return m_clearRequirement; };
	//クリア条件テキスト
	std::wstring GetClearRequirementText()const { return m_clearRequirementText; };
private:
	//変換
	void Conversion() override;
private:
	//チュートリアル名
	std::wstring m_tutorialName;
	//クリア条件
	ClearRequirement m_clearRequirement;
	//クリア条件テキスト
	std::wstring m_clearRequirementText;
};

