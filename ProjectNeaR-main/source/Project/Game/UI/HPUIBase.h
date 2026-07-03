#pragma once
#include "UIBase.h"
#include <memory>
class CharaStatus;
class HPUIBase abstract:
	public UIBase
{
public:
	HPUIBase(std::shared_ptr<CharaStatus> charaStatus);
	~HPUIBase();
protected:
	//プレイヤーのステータス
	std::shared_ptr<CharaStatus> m_charaStatus;

	//前の現在体力
	float m_beforeNowHP;
	//現在の体力
	float m_nowHpRate;

	//ダメージフレーム
	int m_damageFrame;
	//ダメージバーの割合
	float m_damageValueRate;

	//回復フレーム
	int m_healFrame;
	//回復バーの割合
	float m_healValueRate;
protected:
	//体力ハンドル
	int m_nowValueHandle;
	//後ろのバー
	int m_backValueHandle;
	//ダメージ
	int m_damageValueHandle;
	//回復
	int m_healValueHandle;
	//バーのフレーム
	int m_barFrameHandle;
protected:
	//ダメージバー
	void UpdateDamageBar();
	//回復バー
	void UpdateHealBar();
};

