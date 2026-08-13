#include "MagicMove.h"

namespace
{
	// マジックショットの最大移動距離
	constexpr float SHOT_DISTANCE_MAX = 3000.0f;
	// マジックミサイルの最大移動距離
	constexpr float MISSILE_DISTANCE_MAX = 5000.0f;
	// マジックビームの最大移動距離
	constexpr float BEAM_DISTANCE_MAX = 5000.0f;
	// 目標位置の高さ補正用
	constexpr float HEIGHT_OFFSET = 300.0f;
	// 加速度
	constexpr float ACCEL_RATE = 25.0f;
	// 加速度の補正
	constexpr float ACCEL_OFFSET = 500.0f;
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
	VECTOR toTarget = VSub(VGet(targetPos.x, HEIGHT_OFFSET, targetPos.z), data.pos);
	VECTOR toTargetDir = VNorm(toTarget);
	float toTargetDist = VSize(toTarget);

	float rate = ACCEL_OFFSET / toTargetDist;

	data.velo = VAdd(data.velo, VScale(toTargetDir, ACCEL_RATE * rate));

	// 座標を更新	
	data.pos = VAdd(data.pos, data.velo);
	data.movedDistance += VSize(data.velo);

	// 最大距離まで移動したら消す
	if (data.movedDistance >= MISSILE_DISTANCE_MAX)
	{
		data.isExist = false;
		data.movedDistance = 0.0f;
	}
}

void MagicMove::BeamMove(MagicBase::MagicData& data, VECTOR targetPos, VECTOR startPos)
{
	data.segmentStPos = startPos;
	data.segmentEndPos = VAdd(data.segmentEndPos, VScale(data.moveDirection, data.speed));
	data.movedDistance += data.speed;

	if (data.movedDistance >= BEAM_DISTANCE_MAX)
	{
		data.isExist = false;
		data.movedDistance = 0.0f;
	}
}

void MagicMove::FuryMove(MagicBase::MagicData& data, VECTOR targetPos)
{
	data.segmentEndPos = VAdd(data.segmentEndPos, VScale(data.moveDirection, data.speed));

	if (data.segmentEndPos.y <= targetPos.y)
	{
		data.isArrived = true;
		data.movedDistance = 0.0f;
	}
}
