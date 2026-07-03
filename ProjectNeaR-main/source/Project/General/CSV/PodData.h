#pragma once
#include "CSVData.h"
#include <memory>
#include <string>
#include "../Math/MyMath.h"
class PodData :
	public CSVData
{
public:
	PodData();
	PodData(std::shared_ptr<CSVData> data);
	~PodData();
	//大きさ
	Vector3 GetScale()const { return m_scale; };
	//モデルのパス
	std::wstring GetModelPath()const { return m_modelPath; };
	//攻撃力
	int GetAT()const { return m_at; };
private:
	//変換
	void Conversion() override;

	//大きさ
	Vector3 m_scale;
	//モデルのパス
	std::wstring m_modelPath;
	//攻撃力
	int m_at;
};

