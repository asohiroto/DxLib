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
	void Approach(std::shared_ptr<Enemy> pEnemy);
	// 離れる
	void MoveAway(std::shared_ptr<Enemy> pEnemy);
	// 左右移動
	void MoveLR(std::shared_ptr<Enemy> pEnemy);
	// 距離と方向を計算する
	void CalDistDir(VECTOR playerPos, std::shared_ptr<Enemy> pEnemy);

private:
	// プレイヤーまでの距離
	float _toPlayerDistance;
	// プレイヤーのいる方向
	VECTOR _toPlayerDir;
	// 遠すぎる
	bool _tooAway;
	// 近すぎる
	bool _tooNear;
};