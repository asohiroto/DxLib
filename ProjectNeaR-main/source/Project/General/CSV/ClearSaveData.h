#pragma once
#include "CSVData.h"
#include <memory>
#include <string>
class ClearSaveData :
    public CSVData
{
public:
	ClearSaveData();
	ClearSaveData(std::shared_ptr<CSVData> data);
	~ClearSaveData();

	//リセット
	void Reset();

	//クリアしたか
	bool IsClearTutorial1()const { return m_isClearTutorial1; };
	bool IsClearTutorial2()const { return m_isClearTutorial2; };
	bool IsClearTutorial3()const { return m_isClearTutorial3; };
	bool IsClearStage1()const { return m_isClearStage1; };
	bool IsClearStage2()const { return m_isClearStage2; };
	bool IsClearStage3()const { return m_isClearStage3; };

	//クリアした
	void ClearTutorial1() { m_isClearTutorial1 = true; };
	void ClearTutorial2() { m_isClearTutorial2 = true; };
	void ClearTutorial3() { m_isClearTutorial3 = true; };
	void ClearStage1() { m_isClearStage1 = true; };
	void ClearStage2() { m_isClearStage2 = true; };
	void ClearStage3() { m_isClearStage3 = true; };

private:
	//変換
	void Conversion() override;
private:
	//クリアしたか
	bool m_isClearTutorial1;
	bool m_isClearTutorial2;
	bool m_isClearTutorial3;
	bool m_isClearStage1;
	bool m_isClearStage2;
	bool m_isClearStage3;
};

