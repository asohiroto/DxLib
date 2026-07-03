#pragma once
#include <vector>
#include <string>
class CSVData
{
public:
	CSVData();
	~CSVData();
	//データをセット
	void SetData(std::vector<std::wstring> data) { m_data = data; };
	//データを返す
	std::vector<std::wstring> GetData()const { return m_data; };
private:
	//データを変換
	virtual void Conversion() {};
protected:
	std::vector<std::wstring> m_data;
};

