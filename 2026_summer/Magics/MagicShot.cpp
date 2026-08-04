#include "MagicShot.h"
#include "MagicManager.h"
#include <DxLib.h>

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

MagicShot::MagicShot() :
	_magicShot()
{
}

MagicShot::~MagicShot()
{
}

void MagicShot::Init()
{
	// 初期設定
	_magicShot.pos = VGet(0.0f, 0.0f, 0.0f);
	_magicShot.radius = RADIUS;
	_magicShot.speed = SPEED;
	_magicShot.type = MagicType::MagicShot;
	_magicShot.isExist = false;
	_magicShot.isEnemy = false;
}

void MagicShot::End()
{
}

void MagicShot::Update()
{

}

void MagicShot::Draw()
{
}

void MagicShot::GenerateShot(VECTOR pos, VECTOR front, bool isEnemy, std::shared_ptr<MagicManager> pManager)
{
	VECTOR tempPos = VAdd(pos, VScale(VNorm(front), POSITION_OFFSET));
	tempPos.y += HEIGHT_OFFSET;
	_magicShot.pos = tempPos;
	_magicShot.isExist = true;
	_magicShot.isEnemy = isEnemy;
	_magicShot.moveDirection = VNorm(front);

	pManager->EntryList(_magicShot);
}