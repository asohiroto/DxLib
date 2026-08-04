#pragma once
#include "Bases/Character.h"

class Enemy : public Character
{
public:
	enum EnemyState
	{
		// プレイヤーとの距離を調整
		Chase,
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
	void Update() override;
	void Draw() override;

	// エネミーデータのゲッター
	CharacterData GetEnemyData() const { return _enemyUnit; }
	// 当たった処理
	void SetHit(bool judge) { _enemyUnit.isHit = judge; }
	// 当たり判定の色を変更
	void SetColor(int col) { _enemyUnit.color = col; }
	// 敵座標のゲッター
	VECTOR GetPos() const { return _enemyUnit.pos; }
	// ステートを変更
	void ChangeState(EnemyState state) { _enemyUnit.state = state; }

private:
	CharacterData _enemyUnit;
};