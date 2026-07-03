#pragma once
#include "CSVData.h"
#include <memory>
#include <string>
#include "../Math/MyMath.h"
class TextData :
	public CSVData
{
public:
	TextData();
	TextData(std::shared_ptr<CSVData> data);
	~TextData();
	//ID
	std::wstring GetID() const { return m_id; };
	//テキスト
	std::wstring GetText()const { return m_text; };
	//話してる人
	std::wstring GetSpeaker()const { return m_speaker; };
	//次のテキストID
	std::wstring GetNextID()const { return m_nextID; };
	//グループ
	std::wstring GetGroup()const { return m_group; };
	//画像パス
	std::wstring GetImagePath()const { return m_imagePath; };
private:
	//変換
	void Conversion() override;
private:
	//ID
	std::wstring m_id;
	//テキスト
	std::wstring m_text;
	//話してる人
	std::wstring m_speaker;
	//次のテキストID
	std::wstring m_nextID;
	//グループ
	std::wstring m_group;
	//画像ハンドル
	std::wstring m_imagePath;
};

