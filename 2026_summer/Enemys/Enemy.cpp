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
	_tempEnemyUnit.radius = RADIUS;

	// モデルを拡大
	MV1SetScale(_tempEnemyUnit.modelH, VGet(3.0f, 3.0f, 3.0f));
}

void Enemy::End()
{
}

void Enemy::Update()
{
	// 【当たり判定用】線分の始点と終点を設定
	_tempEnemyUnit.segmentStPos = VGet(_tempEnemyUnit.pos.x, SEGMENT_HEIGHT_COR, _tempEnemyUnit.pos.z);
	_tempEnemyUnit.segmentEndPos = VGet(_tempEnemyUnit.pos.x, SEGMENT_HEIGHT_COR + SEGMENT_LENGTH, _tempEnemyUnit.pos.z);

}

void Enemy::Draw()
{
	MV1SetPosition(_tempEnemyUnit.modelH, _tempEnemyUnit.pos);
	MV1DrawModel(_tempEnemyUnit.modelH);
#ifdef _DEBUG
	DrawCapsule3D(_tempEnemyUnit.segmentStPos, _tempEnemyUnit.segmentEndPos, _tempEnemyUnit.radius, 16, 0xff0000, 0xff0000, false);
#endif
}
