#include "MagicShot.h"
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
	constexpr float POSITION_COR = 50.0f;
	// 生成高さの補正用
	constexpr float HEIGHT_COR = 250.0f;
}

MagicShot::MagicShot() :
	_magicShot(),
	_movedDistance(0.0f),
	_isInitialize(true),
	_distDir(VGet(0.0f, 0.0f, 0.0f))
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
#ifdef _DEBUG
	DrawSphere3D(_magicShot.pos, _magicShot.radius, 16, 0xffffff, 0xffffff, true);
#endif
}

void MagicShot::GenerateShot(VECTOR pos, VECTOR front, bool isEnemy)
{
	VECTOR tempPos = VAdd(pos, VScale(VNorm(front), POSITION_COR));
	tempPos.y += HEIGHT_COR;
	_magicShot.pos = tempPos;
	_magicShot.isExist = true;
	_magicShot.isEnemy = isEnemy;
}

void MagicShot::Move(VECTOR dir)
{
	// 初期化
	if (_isInitialize)
	{
		_distDir = VNorm(dir);
		_isInitialize = false;
	}

	// 座標を更新
	_magicShot.pos = VAdd(_magicShot.pos, VScale(_distDir, _magicShot.speed));
	_movedDistance += _magicShot.speed;

	// 最大距離まで移動したら消す
	if (_movedDistance >= DISTANCE_MAX)
	{
		_magicShot.isExist = false;
		_magicShot.pos = VGet(0.0f, 0.0f, 0.0f);
		_movedDistance = 0.0f;
		_distDir = VGet(0.0f, 0.0f, 0.0f);
		_isInitialize = true;
	}
}