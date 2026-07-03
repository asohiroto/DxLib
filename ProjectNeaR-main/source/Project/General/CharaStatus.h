#pragma once
#include "Math/MyMath.h"
#include <memory>
class CharaStatusData;
//ステータスを管理します
class CharaStatus
{
public:
	CharaStatus(std::shared_ptr<CharaStatusData> charaStatusData);
	~CharaStatus();

	//初期化
	void Init();

	//ヒット状態の初期化
	void InitHitState();
	//攻撃の重さ
	enum class AttackWeight : int
	{
		Light = 0,
		Middle = 1,
		Heavy = 2,
		Heaviest = 3,
	};
	//攻撃でひるまない強さ
	typedef CharaStatus::AttackWeight Armor;
	/// <summary>
	/// 攻撃とアーマーの比較
	/// 攻撃のほうが大きいならtrue
	/// </summary>
	/// <param name="aw"></param>
	/// <param name="am"></param>
	/// <returns></returns>
	bool CheckHitReaction(CharaStatus::AttackWeight aw, CharaStatus::Armor am);

	//アーマー
	CharaStatus::Armor GetArmor()const { return m_ar; };
	void SetArmor(CharaStatus::Armor ar) { m_ar = ar; };
	//アーマーの初期化
	void InitArmor();

	//体力
	int GetMaxHP()const { return m_maxHp; };
	void SetMaxHP(unsigned int maxHP) { m_maxHp = maxHP; };
	int GetNowHP()const { return m_nowHp; };
	void SetNowHP(unsigned int nowHP) { m_nowHp = nowHP; };
	float GetHPRate()const;

	//ダメージを受けた
	bool IsDamaged()const { return m_nowHp < m_beforeHp; };

	//攻撃力
	int GetAT()const { return m_at; };
	void SetAT(int at) { m_at = at; };

	//防御力
	int GetDF()const { return m_df; };
	void SetDF(int df) { m_df = df; };

	//移動速度
	float GetMS()const { return m_ms; };
	void SetMS(float ms) { m_ms = ms; };

	//ジャンプ力
	float GetJP()const { return m_jp; };
	void SetJP(float jp) { m_jp = jp; };

	//ダメージ
	void OnDamage(int power,int at, CharaStatus::AttackWeight aw);
	//ノックバック
	Vector3 GetKnockBack(const Vector3& charPos, const Vector3& attackPos, float power, float up);

	//回復
	void Heal(int value);
	//完全回復
	void FullRecovery();
	//死亡
	bool IsDead()const { return m_nowHp <= 0; };
	//攻撃を喰らったことをなかったことに
	void ResetDamage();

	//無敵
	void SetIsNoDamage(bool isNoDamage) { m_isNoDamage = isNoDamage; };
	bool IsNoDamage()const { return m_isNoDamage; };
	//攻撃を受けたか
	bool IsHit()const { return m_isHit; }
	void SetIsHit(bool isHit) { m_isHit = isHit; }
	//リアクションをするか
	bool IsHitReaction()const { return m_isHitReaction; };

	//索敵半径
	float GetSearchRange()const { return m_searchRange; };
	float GetSearchAngle()const { return m_searchAngle; };
	float GetMeleeAttackRange()const { return m_meleeAttackRange; };
	void SetMeleeAttackRange(float meleeAttackRange){  m_meleeAttackRange = meleeAttackRange; };

	//ピンチ
	bool IsPinchHP()const;

private:
	//ステータス
	unsigned int		m_maxHp;	//最大体力
	unsigned int		m_nowHp;	//現在体力
	int					m_at;		//攻撃力
	int					m_df;		//防御力
	float				m_ms;		//移動速度
	float				m_jp;		//ジャンプ力
	CharaStatus::Armor  m_ar;		//アーマー
	float 			m_searchRange; //索敵半径
	float 			m_searchAngle; //索敵アングル
	float 			m_meleeAttackRange; //近接攻撃距離
	
	//状態
	//無敵
	bool m_isNoDamage;
	//攻撃を受けた
	bool m_isHit;
	//リアクションをするか
	bool m_isHitReaction;

	//初期値
	std::shared_ptr<CharaStatusData> m_charaStatusData;

	//持ち主
	Vector3 m_ownerPos;

	//1フレーム前の体力
	unsigned int m_beforeHp;
private:
	//ダメージ計算
	int GetDamage(int power, int at);
};
//演算子オーバーロード
CharaStatus::AttackWeight operator+(const CharaStatus::AttackWeight& left, const CharaStatus::AttackWeight& right);
CharaStatus::AttackWeight operator+(const CharaStatus::AttackWeight& left, const int right);
