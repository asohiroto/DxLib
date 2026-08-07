#pragma once
#include <DxLib.h>
#include <memory>

class Enemy;

class EnemyMove
{
public:
	EnemyMove();
	virtual ~EnemyMove();
	void Init();
	void End();
	void Update(VECTOR playerPos, std::shared_ptr<Enemy> pEnemy);
	void Draw();
	// 近づく
	void Approach();
	// 離れる
	void MoveAway();
	// 左右移動
	void MoveLR();

private:
	// プレイヤーまでの距離
	float _toPlayerDistance;
};