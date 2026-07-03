#pragma once
#include "CSVData.h"
#include <memory>
#include <string>
#include "../Math/MyMath.h"
#include "../CharaStatus.h"
class ArmData :
	public CSVData
{
public:
	ArmData();
	ArmData(std::shared_ptr<CSVData> data);
	~ArmData();

	//名前
	std::wstring GetName() const{ return m_name; };
	//体力
	int GetHP()const { return m_hp; };
	//防御
	int GetDF()const { return m_df; };
	//アーマー
	CharaStatus::Armor GetArmor()const { return m_ar; };
	//半径
	float GetRadius()const { return m_radius; };
	//始点インデックス
	int GetStartIndex()const { return m_startIndex; };
	//終点インデックス
	int GetEndIndex()const { return m_endIndex; };
private:
	//変換
	void Conversion() override;

	//名前
	std::wstring m_name;
	//体力
	int m_hp;
	//防御
	int m_df;
	//アーマー
	CharaStatus::Armor m_ar;
	//半径
	float m_radius;
	//始点インデックス
	int m_startIndex;
	//終点インデックス
	int m_endIndex;
};

