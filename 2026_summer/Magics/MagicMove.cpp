#include "MagicMove.h"

namespace
{
	// マジックショットの最大移動距離
	constexpr float SHOT_DISTANCE_MAX = 3000.0f;
	// マジックミサイルの最大移動距離
	constexpr float MISSILE_DISTANCE_MAX = 5000.0f;
	// 目標位置の高さ補正用
	constexpr float HEIGHT_OFFSET = 300.0f;
	// 加速度
	constexpr float ACCEL_RATE = 50.0f;
	// 初速度
	constexpr float FIRST_RATE = 150.0f;
}

MagicMove::MagicMove()
{
}

MagicMove::~MagicMove()
{
}

void MagicMove::Init()
{
}

void MagicMove::End()
{
}

void MagicMove::Update()
{
}

void MagicMove::Draw()
{
}

void MagicMove::ShotMove(MagicBase::MagicData& data)
{
	// 座標を更新
	data.pos = VAdd(data.pos, VScale(data.moveDirection, data.speed));
	data.movedDistance += data.speed;

	// 最大距離まで移動したら消す
	if (data.movedDistance >= SHOT_DISTANCE_MAX)
	{
		data.isExist = false;
		data.movedDistance = 0.0f;
	}
}

void MagicMove::MissileMove(MagicBase::MagicData& data, VECTOR targetPos)
{

	VECTOR velo = VScale(data.moveDirection, data.speed);
	VECTOR toTarget = VNorm(VSub(VGet(targetPos.x, HEIGHT_OFFSET, targetPos.z), data.pos));

	velo = VAdd(velo, VScale(toTarget, ACCEL_RATE));

	// 座標を更新	
	data.pos = VAdd(data.pos, velo);
	data.movedDistance += VSize(velo);

	// 最大距離まで移動したら消す
	if (data.movedDistance >= MISSILE_DISTANCE_MAX)
	{
		data.isExist = false;
		data.movedDistance = 0.0f;
	}
}
