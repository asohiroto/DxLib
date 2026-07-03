#pragma once
#include <DxLib.h>
#include <string>
#include <vector>
#include <memory>
class CSVData;
class CSVDataSaver final
{
private:
	//シングルトンの準備
	CSVDataSaver() = default;
	~CSVDataSaver() = default;
	//コピー禁止
	CSVDataSaver(const CSVDataSaver&) = delete;
	CSVDataSaver& operator = (const CSVDataSaver&) = delete;
	//ムーブ禁止
	CSVDataSaver(CSVDataSaver&&) = delete;
	CSVDataSaver& operator = (CSVDataSaver&&) = delete;
public:
	//インスタンスを取得
	static CSVDataSaver& GetInstance()
	{
		static CSVDataSaver instance;
		return instance;
	}
		
	//セーブ
	void SaveOption();
	void SaveClearData();
	void SaveHighScoreData();
	void SaveTimeData();
private:
	
};