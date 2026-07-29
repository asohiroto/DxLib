#include "Enemy.h"

Enemy::Enemy() :
	_tempEnemyUnit()
{
}

Enemy::~Enemy()
{
	MV1DeleteModel(_tempEnemyUnit.modelH);
}

void Enemy::Init()
{

	_tempEnemyUnit.pos = VGet(0.0f, 0.0f, 0.0f);
	_tempEnemyUnit.modelH = MV1LoadModel("data/Model_army.mv1");

	// モデルを拡大
	MV1SetScale(_tempEnemyUnit.modelH, VGet(3.0f, 3.0f, 3.0f));
}

void Enemy::End()
{
}

void Enemy::Update()
{
}

void Enemy::Draw()
{
	MV1SetPosition(_tempEnemyUnit.modelH, _tempEnemyUnit.pos);
	MV1DrawModel(_tempEnemyUnit.modelH);
}
