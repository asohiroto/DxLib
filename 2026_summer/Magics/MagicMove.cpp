#include "MagicMove.h"
#include <EffekseerForDXLib.h>
#include <cmath>

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
	// マジックフューリーに欠ける線形補間度
	constexpr float FURY_LERP_RATE = 0.15f;
	// 到着したとみなす幅
	constexpr float ARRIVED_LENGTH = 0.05f;
	// ビームを生成する時間
	constexpr int BEAM_EXIST_COUNT = 30;
	// ビームをチャージする時間
	constexpr int BEAM_CHARGE_COUNT = 10;
	// マジックビームが目標地点を定めてから攻撃を行うまでの猶予時間
	constexpr int LOCKON_DELAY = 4;
	// マジックビームの長さの倍率
	constexpr float BEAM_LENGTH_SCALE = 1.5f;
}

MagicMove::MagicMove() :
	_beamTargetPos(VGet(0.0f, 0.0f, 0.0f))
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

	if (data.effectH != -1)
		SetPosPlayingEffekseer3DEffect(data.effectH, data.pos.x, data.pos.y, data.pos.z);

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

	if (data.effectH != -1)
		SetPosPlayingEffekseer3DEffect(data.effectH, data.pos.x, data.pos.y, data.pos.z);

	// 最大距離まで移動したら消す
	if (data.movedDistance >= MISSILE_DISTANCE_MAX)
	{
		data.isExist = false;
		data.movedDistance = 0.0f;
	}
}

void MagicMove::BeamMove(MagicBase::MagicData& data, VECTOR targetPos, VECTOR startPos)
{
	data.chargeCount++;
	data.existCount++;

	float dirAngle = atan2f(data.moveDirection.x, data.moveDirection.z);

	SetPosPlayingEffekseer3DEffect(data.effectH, startPos.x, startPos.y, startPos.z);
	SetRotationPlayingEffekseer3DEffect(data.effectH, 0.0f, dirAngle + DX_PI_F, 0.0f);

	if (data.chargeCount < BEAM_CHARGE_COUNT - LOCKON_DELAY)
	{
		_beamTargetPos = VScale(targetPos, BEAM_LENGTH_SCALE);
		data.segmentEndPos = data.segmentStPos;
	}
	else if (data.chargeCount > BEAM_CHARGE_COUNT + (LOCKON_DELAY * 3))
	{
		data.segmentStPos = startPos;
		if (data.isEnemy)
			data.segmentEndPos = VGet(_beamTargetPos.x, _beamTargetPos.y + HEIGHT_OFFSET, _beamTargetPos.z);
		else
			data.segmentEndPos = targetPos;

		if (data.existCount >= BEAM_EXIST_COUNT)
		{
			data.isExist = false;
			data.movedDistance = 0.0f;
			data.chargeCount = 0;
			data.existCount = 0;
		}
	}
}

void MagicMove::FuryMove(MagicBase::MagicData& data, VECTOR targetPos)
{
	data.segmentEndPos = VAdd(data.segmentEndPos, VScale(VSub(targetPos, data.segmentEndPos), FURY_LERP_RATE));

	//if (data.effectH != -1)
	//	SetPosPlayingEffekseer3DEffect(data.effectH, targetPos.x, 0.0f, targetPos.z);

	SetPosPlayingEffekseer3DEffect(data.effectH, data.segmentEndPos.x, data.segmentEndPos.y, data.segmentEndPos.z);

	if (data.segmentEndPos.y - targetPos.y <= ARRIVED_LENGTH)
	{
		data.isArrived = true;
		data.movedDistance = 0.0f;
	}
}
