#include "PlayerDodge.h"
#include "Inputs/Input.h"

namespace
{
	// 回避速度
	constexpr float SPEED = 20.0f;
	// 最大回避距離
	constexpr float MAX_DISTANCE = 200.0f;
}

PlayerDodge::PlayerDodge() :
	_dodgeDir(VGet(0.0f, 0.0f, 0.0f)),
	_isDodge(false),
	_dodgeDistance(0.0f),
	_dodgedPos(VGet(0.0f, 0.0f, 0.0f))
{
}

PlayerDodge::~PlayerDodge()
{
}

void PlayerDodge::Init()
{
}

void PlayerDodge::End()
{
}

void PlayerDodge::Update(std::shared_ptr<Input> pInput, float cameraAngle)
{
	CalDirection(pInput, cameraAngle);

	if (pInput->IsTrigger(PAD_INPUT_4))
	{
		_isDodge = true;
	}

	if (_isDodge)
	{
		_dodgeDistance += SPEED;
		if (_dodgeDistance <= MAX_DISTANCE)
		{
			_dodgedPos = VAdd(_dodgedPos, VScale(_dodgeDir, SPEED));
		}
		else if (_dodgeDistance > MAX_DISTANCE)
		{
			_dodgedPos = VGet(0.0f, 0.0f, 0.0f);
			_isDodge = false;
			_dodgeDistance = 0.0f;
		}
	}
}

void PlayerDodge::Draw()
{
}

void PlayerDodge::CalDirection(std::shared_ptr<Input> pInput, float cameraAngle)
{
	// 移動の方向に代入
	int _mx = pInput->GetLeftStickX();
	int _mz = pInput->GetLeftStickY();

	// 回避方向を決定
	_dodgeDir = VGet(static_cast<float>(_mx), 0.0f, static_cast<float>(-_mz));

	// 正規化し、方向を取得
	_dodgeDir = VNorm(_dodgeDir);

	// カメラのアングルを行列に変換
	MATRIX _rotMatrix = MGetRotY(cameraAngle);

	// 移動方向を決定
	_dodgeDir = VTransform(_dodgeDir, _rotMatrix);
}
