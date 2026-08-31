#include "EnemyMove.h"
#include "Enemy.h"

namespace
{
	// 移動速度
	constexpr float SPEED = 10.0f;
	// 左右移動時間
	constexpr int TIMER = 60;
	// 左右に動く距離
	constexpr float MOVE_LR_DISTANCE = 500.0f;
	// 近づく距離
	constexpr float APPROACH_DISTANCE = 500.0f;
	// 遠ざかる距離
	constexpr float MOVE_AWAY_DISTANCE = 500.0f;
}

EnemyMove::EnemyMove() :
	_toPlayerDistance(0.0f),
	_toPlayerDir(VGet(0.0f, 0.0f, 0.0f)),
	_tooAway(false),
	_tooNear(false),
	_isGoLeft(false),
	_moveLR(false),
	_isActionFinished(true),
	_movedDistance(0.0f)
{
}

EnemyMove::~EnemyMove()
{
}

void EnemyMove::Init()
{
}

void EnemyMove::End()
{
}

void EnemyMove::Update(VECTOR playerPos, std::shared_ptr<Enemy> pEnemy)
{
	// 距離と方向を計算し代入
	CalDistDir(playerPos, pEnemy);
}

void EnemyMove::Draw()
{
}

void EnemyMove::Approach(std::shared_ptr<Enemy> pEnemy)
{
	pEnemy->SetPos(VAdd(pEnemy->GetPos(), VScale(_toPlayerDir, SPEED)));

	_movedDistance += SPEED;

	if (_movedDistance >= APPROACH_DISTANCE)
	{
		_isActionFinished = true;
		_movedDistance = 0.0f;
	}
}

void EnemyMove::MoveAway(std::shared_ptr<Enemy> pEnemy)
{
	VECTOR opposite = VGet(-_toPlayerDir.x, 0.0f, -_toPlayerDir.z);
	pEnemy->SetPos(VAdd(pEnemy->GetPos(), VScale(opposite, SPEED)));

	_movedDistance += SPEED;

	if (_movedDistance >= MOVE_AWAY_DISTANCE)
	{
		_isActionFinished = true;
		_movedDistance = 0.0f;
	}
}

void EnemyMove::MoveLeft(std::shared_ptr<Enemy> pEnemy)
{
	// 左側ベクトル
	VECTOR left = VGet(-_toPlayerDir.z, 0.0f, _toPlayerDir.x);

	pEnemy->SetPos(VAdd(pEnemy->GetPos(), VScale(left, SPEED)));

	_movedDistance += SPEED;

	if (_movedDistance >= MOVE_LR_DISTANCE)
	{
		_isActionFinished = true;
		_movedDistance = 0.0f;
	}
}

void EnemyMove::MoveRight(std::shared_ptr<Enemy> pEnemy)
{
	// 右側ベクトル
	VECTOR right = VGet(_toPlayerDir.z, 0.0f, -_toPlayerDir.x);

	pEnemy->SetPos(VAdd(pEnemy->GetPos(), VScale(right, SPEED)));

	_movedDistance += SPEED;

	if (_movedDistance >= MOVE_LR_DISTANCE)
	{
		_isActionFinished = true;
		_movedDistance = 0.0f;
	}
}

void EnemyMove::CalDistDir(VECTOR playerPos, std::shared_ptr<Enemy> pEnemy)
{
	// エネミーからプレイヤーへのベクトルをとる
	_toPlayerDir = VSub(playerPos, pEnemy->GetPos());
	// その大きさを計算
	_toPlayerDistance = VSize(_toPlayerDir);
	// 正規化して、方向を計算
	_toPlayerDir = VNorm(_toPlayerDir);
}

