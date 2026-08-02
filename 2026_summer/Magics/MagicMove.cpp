#include "MagicMove.h"

namespace
{
	// マジックショットの最大移動距離
	constexpr float SHOT_DISTANCE_MAX = 1500.0f;
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
