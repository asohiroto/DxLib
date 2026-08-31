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
	// 左移動
	void MoveLeft(std::shared_ptr<Enemy> pEnemy);
	// 右移動
	void MoveRight(std::shared_ptr<Enemy> pEnemy);
	// 距離と方向を計算する
	void CalDistDir(VECTOR playerPos, std::shared_ptr<Enemy> pEnemy);
	// プレイヤー方向のゲッター
	VECTOR GetDir() const { return _toPlayerDir; }
	// 行動を終了したか
	bool IsActionFinished() const { return _isActionFinished; }
	// フラグを変更
	void SetActionFinished(bool judge) { _isActionFinished = judge; }
	// プレイヤーまでの距離のゲッター
	float GetDistance() const { return _toPlayerDistance; }

private:
	// プレイヤーまでの距離
	float _toPlayerDistance;
	// プレイヤーのいる方向
	VECTOR _toPlayerDir;
	// 遠すぎる
	bool _tooAway;
	// 近すぎる
	bool _tooNear;
	// 左に移動するか
	bool _isGoLeft;
	// 左右移動を行うか
	bool _moveLR;
	// 行動完了フラグ
	bool _isActionFinished;
	// 移動距離
	float _movedDistance;
};