#include "Enemy.h"

namespace
{
	// 敵の半径
	constexpr float RADIUS = 130.0f;
	// 線分の長さ
	constexpr float SEGMENT_LENGTH = 270.0f;
	// 線分を高く補正する
	constexpr float SEGMENT_HEIGHT_COR = 100.0f;
}

Enemy::Enemy() :
	_enemyUnit(),
	_nowState(EnemyState::Move)
{
}

Enemy::~Enemy()
{
	MV1DeleteModel(_enemyUnit.modelH);
}

void Enemy::Init()
{

	_enemyUnit.pos = VGet(0.0f, 0.0f, 0.0f);
	_enemyUnit.modelH = MV1LoadModel("data/Model_army.mv1");
	_enemyUnit.radius = RADIUS;
	_enemyUnit.color = 0xff0000;
	_nowState = Move;

	// モデルを拡大
	MV1SetScale(_enemyUnit.modelH, VGet(3.0f, 3.0f, 3.0f));
}

void Enemy::End()
{
}

void Enemy::Update()
{
	// 【当たり判定用】線分の始点と終点を設定
	_enemyUnit.segmentStPos = VGet(_enemyUnit.pos.x, SEGMENT_HEIGHT_COR, _enemyUnit.pos.z);
	_enemyUnit.segmentEndPos = VGet(_enemyUnit.pos.x, SEGMENT_HEIGHT_COR + SEGMENT_LENGTH, _enemyUnit.pos.z);

}

void Enemy::Draw()
{
	MV1SetPosition(_enemyUnit.modelH, _enemyUnit.pos);
	MV1DrawModel(_enemyUnit.modelH);
#ifdef _DEBUG
	DrawHitBox(_enemyUnit);
#endif
}
