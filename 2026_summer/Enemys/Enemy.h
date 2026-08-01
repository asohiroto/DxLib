#pragma once
#include "Bases/Character.h"

class Enemy : public Character
{
public:
	Enemy();
	virtual ~Enemy() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;

	// エネミーデータのゲッター
	CharacterData GetEnemyData() const { return _tempEnemyUnit; }
	// 当たった処理
	void SetHit(bool judge) { _tempEnemyUnit.isHit = judge; }
	// 当たり判定の色を変更
	void SetColor(int col) { _tempEnemyUnit.color = col; }

private:
	CharacterData _tempEnemyUnit;
};