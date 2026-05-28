#pragma once
#include"Vec2.h"
#include<vector>
#include<string>

struct UnitData {
	int soldierCount;	// 兵数
	int attack;			// 攻撃力
	int handle;			// 見た目
	float speed;		// 行軍速度
	bool isEnemy;		// 敵か否か
	std::string name;		// 部隊名
};

/// <summary>
/// キャラクターの基底クラス
/// </summary>
class Character
{
public:
	bool m_isRight; // キャラクターが右を向いているか

	float m_angle;  // キャラクターの向いている角度

	Vec2 m_pos;		// 座標

	std::vector<UnitData> units; // それぞれの軍の部隊を管理する配列

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Character();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Character();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw(float x, float y, float angle, int handle);

	// ユニットデータの取り出しを行う関数
	UnitData GetUnitData(int index) const { return units[index]; }

protected:
	/// <summary>
	/// 重力計算
	/// </summary>
	void Gravity();

	Vec2 m_move;	  // 移動量
	Vec2 m_targetPos; // 目的地の座標

	bool m_isMoving;  // 移動中を判定する
};