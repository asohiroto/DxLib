#include "EnemyMove.h"
#include "Enemy.h"

namespace
{
	// プレイヤーとエネミーの最小距離
	constexpr float MIN_DISTANCE = 1000.0f;
	// プレイヤーと敵の最大距離
	constexpr float MAX_DISTANCE = 2500.0f;
	// 移動速度
	constexpr float SPEED = 15.0f;
}

EnemyMove::EnemyMove() :
	_toPlayerDistance(0.0f),
	_toPlayerDir(VGet(0.0f, 0.0f, 0.0f)),
	_tooAway(false),
	_tooNear(false)
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

	// 距離の判定
	if (_toPlayerDistance <= MIN_DISTANCE) _tooNear = true;
	else if (_toPlayerDistance >= MAX_DISTANCE) _tooAway = true;
	else { _tooNear = false; _tooAway = false; }

	if (_tooNear) MoveAway(pEnemy);
	else if (_tooAway) Approach(pEnemy);
	else MoveLR(pEnemy);

}

void EnemyMove::Draw()
{
}

void EnemyMove::Approach(std::shared_ptr<Enemy> pEnemy)
{
	pEnemy->SetPos(VAdd(pEnemy->GetPos(), VScale(_toPlayerDir, SPEED)));
}

void EnemyMove::MoveAway(std::shared_ptr<Enemy> pEnemy)
{
	VECTOR opposite = VGet(-_toPlayerDir.x, 0.0f, -_toPlayerDir.z);
	pEnemy->SetPos(VAdd(pEnemy->GetPos(), VScale(opposite, SPEED)));
}

void EnemyMove::MoveLR(std::shared_ptr<Enemy> pEnemy)
{
	bool toRight = GetRand(1);

	// 右側ベクトル
	VECTOR right = VGet(_toPlayerDir.z, 0.0f, -_toPlayerDir.x);
	VECTOR left = VGet(-_toPlayerDir.z, 0.0f, _toPlayerDir.z);

	if (toRight) pEnemy->SetPos(VAdd(pEnemy->GetPos(), VScale(right, SPEED)));
	else pEnemy->SetPos(VAdd(pEnemy->GetPos(), VScale(left, SPEED)));
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

