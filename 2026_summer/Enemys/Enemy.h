#pragma once
#include "Bases/Character.h"

class Enemy : public Character
{
public:
	enum EnemyState
	{
		// プレイヤーとの距離を調整
		Move,
		// 攻撃（後に追加）
		Attack,
		// 被弾硬直
		HitStun,
		// 死亡
		Dead
	};

	Enemy();
	virtual ~Enemy() override;
	void Init() override;
	void End() override;
	void Update(float angle);
	void Draw() override;

	// エネミーデータのゲッター
	CharacterData GetEnemyData() const { return _enemyUnit; }
	// 当たった瞬間の処理
	void SetHit(int damage);
	// 当たり判定の色を変更
	void SetColor(int col) { _enemyUnit.color = col; }
	// 敵座標のゲッター
	VECTOR GetPos() const { return _enemyUnit.pos; }
	// ステートのゲッター
	EnemyState GetState() const { return _nowState; }
	// ステートを変更
	void ChangeState(EnemyState state) { _nowState = state; }
	// 座標のセッター
	void SetPos(VECTOR pos) { _enemyUnit.pos = pos; }

private:
	CharacterData _enemyUnit;
	// 現在のステート
	EnemyState _nowState;
	// やられのカウンタ
	int _damagedCount;
};