#pragma once
#include "CSVData.h"
#include "../CharaStatus.h"
#include <memory>
class CharaStatusData :
    public CSVData
{
public:
	CharaStatusData();
	CharaStatusData(std::shared_ptr<CSVData> data);
	~CharaStatusData();
	//ステータス
	//ID
	unsigned int GetID()const { return m_id; };
	void SetID(unsigned int id) { m_id = id; };
	//最大体力
	int	GetHP()const { return m_hp; };		
	void SetHP(int hp) { m_hp = hp; };		
	//攻撃力
	int	GetAT()const { return m_at; };
	void SetAT(int at) { m_at = at; };
	//防御力
	int GetDF()const { return m_df; };
	void SetDF(int df) { m_df = df; };
	//移動速度
	float GetMS()const { return m_ms; };
	//ジャンプ力
	float GetJP()const { return m_jp; };
	//アーマー
	CharaStatus::Armor  GetAR()const { return m_ar; };
	void SetAR(CharaStatus::Armor ar) { m_ar = ar; };
	//索敵半径
	float GetSearchRange()const { return m_searchRange; };
	//索敵アングル
	float GetSearchAngle()const { return m_searchAngle; };
	//近接攻撃距離
	float GetMeleeAttackRange()const { return m_meleeAttackRange; };
	//グループタグ
	std::wstring GetGroupTag()const { return m_groupTag; };
private:
	//変換
	void Conversion() override;
	//ステータス
	unsigned int		m_id;		//ID
	int					m_hp;		//最大体力
	int					m_at;		//攻撃力
	int					m_df;		//防御力
	float				m_ms;		//移動速度
	float				m_jp;		//ジャンプ力
	CharaStatus::Armor  m_ar;		//アーマー
	float m_searchRange;			//索敵半径
	float m_searchAngle;			//索敵アングル
	float m_meleeAttackRange;		//近接攻撃距離
	std::wstring m_groupTag;		//グループタグ
};

