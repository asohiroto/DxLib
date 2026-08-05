#include "MagicMissile.h"
#include "MagicManager.h"

namespace
{
	// 当たり判定の半径
	constexpr float RADIUS = 50.0f;
	// 進む速さ
	constexpr float SPEED = 30.0f;
	// 最大移動距離
	constexpr float DISTANCE_MAX = 900.0f;
	// 生成位置の補正用
	constexpr float POSITION_OFFSET = 50.0f;
	// 生成高さの補正用
	constexpr float HEIGHT_OFFSET = 300.0f;
}


MagicMissile::MagicMissile() :
	_magicMissile()
{
}

MagicMissile::~MagicMissile()
{
}

void MagicMissile::Init()
{
	// 初期設定
	_magicMissile.pos = VGet(0.0f, 0.0f, 0.0f);
	_magicMissile.radius = RADIUS;
	_magicMissile.speed = SPEED;
	_magicMissile.type = MagicType::MagicMissile;
	_magicMissile.isExist = false;
	_magicMissile.isEnemy = false;
	_magicMissile.color = 0xff0000;
}

void MagicMissile::End()
{
}

void MagicMissile::Update()
{
}

void MagicMissile::Draw()
{
}

void MagicMissile::GenerateMissile(VECTOR pos, VECTOR front, bool isEnemy, std::shared_ptr<MagicManager> pManager)
{
	VECTOR tempPos = VAdd(pos, VScale(VNorm(front), POSITION_OFFSET));
	tempPos.y += HEIGHT_OFFSET;
	_magicMissile.pos = tempPos;
	_magicMissile.isExist = true;
	_magicMissile.isEnemy = isEnemy;
	_magicMissile.moveDirection = VNorm(front);

	pManager->EntryList(_magicMissile);
}