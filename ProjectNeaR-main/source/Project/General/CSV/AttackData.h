#pragma once
#include "CSVData.h"
#include "../Math/MyMath.h"
#include "../Collision/Collidable.h"
#include <string>
#include "../../General/CharaStatus.h"
class AttackData :
	public CSVData
{
public:
	AttackData();
	AttackData(std::shared_ptr<CSVData> data);
	~AttackData();

	//攻撃の名前
	std::wstring GetName()const { return m_name; };

	//多段ヒットか
	bool IsMultipleHit()const { return m_isMultipleHit; };

	enum class AttackType : int
	{
		None	= 0,
		Sword	= 1, //剣
		Bullet	= 2, //弾
		Throw	= 3, //投げもの
		Punch	= 4, //パンチ
		AOE		= 5, //範囲ダメージ
		Missile	= 6, //ミサイル
		Beam	= 7	 //ビーム
	};
	//攻撃タイプ
	AttackType GetAttackType()const { return m_attackType; };

	//威力
	int GetAttackPower()const { return m_attackPower; };

	//攻撃の重さ
	CharaStatus::AttackWeight GetAttackWeight()const { return m_attackWeight; };

	//アーマー
	CharaStatus::Armor GetArmor()const { return m_armor; };

	//ノックバックの大きさ
	float GetKnockBackPower(){ return m_knockBackPower; };

	//上方向の力
	float GetVerticalPower()const { return m_verticalPower; };

	//発生フレーム
	int GetStartFrame()const { return m_startFrame; };

	//持続フレーム
	int GetKeepFrame()const { return m_keepFrame; };

	//半径
	float GetRadius()const { return m_radius; };

	//長さ
	float GetLength()const { return m_length; };

	//前進速度
	float GetMoveSpeed()const { return m_moveSpeed; };

	//前進フレーム
	int GetMoveFrame()const { return m_moveFrame; };
	
	//アニメーションの名前
	std::wstring GetAnimName()const { return m_animName; };

	//次の攻撃の名前
	std::wstring GetNextAttackName()const { return m_nextAttackName; };

	//攻撃の基準位置
	enum class AttackOriginPosType : int
	{
		Owner = 0, //持ち主の位置
		Center = 1,//攻撃の中心位置
	};
	AttackOriginPosType GetAttackOriginPosType()const { return m_attackOriginPosType; };

	//キャンセル
	int GetCancelFrame()const { return m_cancelFrame; };

	//ヒットストップのフレーム
	int GetHitStopFrame()const { return m_hitStopFrame; };

	//ヒットストップの揺れ
	int GetHitStopShakePower()const { return m_hitStopShakePower; };

	//ヒットエフェクトパス
	std::wstring GetHitEffectPath()const { return m_hitEffectPath; };

	//攻撃エフェクト
	std::wstring GetAttackEffectPath()const { return m_attackEffectPath; };

	//攻撃発生SE
	std::wstring GetSEAppearPath()const { return m_seAppearPath; };

	//ヒットSE
	std::wstring GetSEHitPath()const { return m_seHitPath; };

	//攻撃ボイス
	std::wstring GetVoicePath()const { return m_voicePath; };

	//パラメータ
	float GetParam1()const { return m_param1; };
	float GetParam2()const { return m_param2; };
	float GetParam3()const { return m_param3; };
	float GetParam4()const { return m_param4; };

private:
	//変換
	void Conversion() override;

	//攻撃の名前
	std::wstring m_name;

	//多段ヒットか
	bool m_isMultipleHit;

	//攻撃タイプ
	AttackType m_attackType;

	//威力
	int m_attackPower;

	//攻撃の重さ
	CharaStatus::AttackWeight m_attackWeight;

	//アーマー
	CharaStatus::Armor m_armor;

	//ノックバックの大きさ
	float m_knockBackPower;

	//上方向の力
	float m_verticalPower;

	//発生フレーム
	int m_startFrame;

	//持続フレーム
	int m_keepFrame;

	//半径
	float m_radius;

	//長さ
	float m_length;

	//前進速度
	float m_moveSpeed;

	//前進フレーム
	int m_moveFrame;

	//アニメーションの名前
	std::wstring m_animName;

	//次の攻撃の名前
	std::wstring m_nextAttackName;

	//攻撃の基準位置
	AttackOriginPosType m_attackOriginPosType;

	//キャンセル
	int m_cancelFrame;

	//ヒットストップフレーム
	int m_hitStopFrame;

	//ヒットストップによるカメラの揺れ
	int m_hitStopShakePower;

	//ヒットエフェクトパス
	std::wstring m_hitEffectPath;

	//攻撃エフェクトパス
	std::wstring m_attackEffectPath;

	//攻撃発生SE
	std::wstring m_seAppearPath;

	//ヒットSE
	std::wstring m_seHitPath;

	//攻撃ボイス
	std::wstring m_voicePath;

	//パラメータ
	float m_param1;
	float m_param2;
	float m_param3;
	float m_param4;

};

