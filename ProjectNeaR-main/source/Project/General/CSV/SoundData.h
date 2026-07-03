#pragma once
#include "CSVData.h"
#include <memory>
#include <string>
#include "../Math/MyMath.h"
class SoundData :
	public CSVData
{
public:
	SoundData();
	SoundData(std::shared_ptr<CSVData> data);
	~SoundData();

	//名前
	std::wstring GetName()const { return m_name; };

	//パス
	std::wstring GetPath()const { return m_path; };

private:
	//変換
	void Conversion() override;

	//名前
	std::wstring m_name;

	//パス
	std::wstring m_path;
};

