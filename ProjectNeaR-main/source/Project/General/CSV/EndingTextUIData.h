#pragma once
#include "CSVData.h"
#include <memory>
#include <string>
#include "../Math/MyMath.h"
#include "../AssetManager.h"
class EndingTextUIData :
	public CSVData
{
public:
	EndingTextUIData();
	EndingTextUIData(std::shared_ptr<CSVData> data);
	~EndingTextUIData();

	//名前
	std::wstring GetText()const { return m_text; };

	//パス
	AssetManager::Font GetFont()const { return m_font; };

private:
	//変換
	void Conversion() override;

	//テキスト
	std::wstring m_text;

	//フォントデータ
	AssetManager::Font m_font;
};

