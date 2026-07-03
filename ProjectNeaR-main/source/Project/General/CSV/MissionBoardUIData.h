#pragma once
#include "CSVData.h"
#include <memory>
#include <string>
#include "../Math/MyMath.h"
class MissionBoardUIData :
	public CSVData
{
public:
	MissionBoardUIData();
	MissionBoardUIData(std::shared_ptr<CSVData> data);
	~MissionBoardUIData();

	//パス
	std::wstring GetPath()const { return m_path; };

	//テキスト
	std::wstring GetText()const { return m_text; };

	//タイトル
	std::wstring GetTitle()const { return m_title; };

private:
	//変換
	void Conversion() override;

	//パス
	std::wstring m_path;

	//テキスト
	std::wstring m_text;

	//タイトル
	std::wstring m_title;
};

