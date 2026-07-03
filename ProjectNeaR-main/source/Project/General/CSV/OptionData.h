#pragma once
#include "CSVData.h"
#include <memory>
class OptionData :
    public CSVData
{
public:
	OptionData();
	OptionData(std::shared_ptr<CSVData> data);
	~OptionData();

	//音量
	int GetBGMLevel()const { return m_bgmLv; };
	int GetSELevel()const { return m_seLv; };
	int GetVoiceLevel()const { return m_voiceLv; };
	//スクリーンモード
	bool IsWindow()const { return m_isWindow; };
	
private:
	//変換
	void Conversion() override;

private:
	//音量
	int m_bgmLv;
	int m_seLv;
	int m_voiceLv;
	//スクリーンモード
	bool m_isWindow;
};

