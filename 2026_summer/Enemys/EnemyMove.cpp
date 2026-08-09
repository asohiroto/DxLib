#include "EnemyMove.h"
#include "Enemy.h"

namespace
{
	// プレイヤーとエネミーの最小距離
	constexpr float MIN_DISTANCE = 500.0f;
	// プレイヤーと敵の最大距離
	constexpr float MAX_DISTANCE = 1500.0f;
}

EnemyMove::EnemyMove() :
	_toPlayerDistance(0.0f),
	_toPlayerDir(VGet(0.0f, 0.0f, 0.0f))
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
}

void EnemyMove::MoveAway(std::shared_ptr<Enemy> pEnemy)
{
}

void EnemyMove::MoveLR(std::shared_ptr<Enemy> pEnemy)
{
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

