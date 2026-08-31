#pragma once
#include "Bases/Character.h"
#include <memory>

class EnemyManager;
class AnimManager;

class Enemy : public Character
{
public:
	Enemy();
	virtual ~Enemy() override;
	void Init(int handle, int score);
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
	CharacterState GetState() const { return _enemyUnit.nowState; }
	// ステートを変更
	void ChangeState(CharacterState state);
	// 座標のセッター
	void SetPos(VECTOR pos) { _enemyUnit.pos = pos; }
	// 最大HPのゲッター
	int GetMaxHp() const { return _enemyUnit.maxHp; }
	// 現在HPのゲッター
	int GetNowHp() const { return _enemyUnit.hp; }

private:
	// アニメマネージャーの共有ポインタ
	std::shared_ptr<AnimManager> p_AManager;
	CharacterData _enemyUnit;
	// やられのカウンタ
	int _damagedCount;
};