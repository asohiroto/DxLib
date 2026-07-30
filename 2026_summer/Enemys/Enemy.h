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
	void OnHit() { _tempEnemyUnit.isHit = true; }

private:
	CharacterData _tempEnemyUnit;
};