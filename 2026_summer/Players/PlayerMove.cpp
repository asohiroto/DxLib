#include "PlayerMove.h"
#include "Inputs/Input.h"
#include <algorithm>

namespace
{
	// プレイヤーの移動速度
	constexpr float MOVE_SPEED = 12.0f;
	// 入力値変換用の値
	constexpr float INPUT_COR = 0.001f;
}

PlayerMove::PlayerMove() :
	_move(VGet(0.0f, 0.0f, 0.0f)),
	_mx(0), _mz(0),
	_movementDirection(VGet(0.0f, 0.0f, 0.0f)),
	_rotMatrix()
{
}

PlayerMove::~PlayerMove()
{
}

void PlayerMove::Init()
{
}

void PlayerMove::End()
{
}

void PlayerMove::Update(std::shared_ptr<Input> pInput, float cameraAngle)
{
	// 移動の方向に代入
	_mx = pInput->GetLeftStickX();
	_mz = pInput->GetLeftStickY();

	// 移動量に代入
	_move = VGet(static_cast<float>(_mx), 0.0f, static_cast<float>(-_mz));

	// 入力値（傾き具合）を取得
	float length = VSize(_move);

	// 0.0~1.0の間の値に変換
	float rate = length * INPUT_COR;
	rate = std::clamp(rate, 0.0f, 1.0f);

	// 正規化し、方向を取得
	_move = VNorm(_move);

	// 入力値を考慮した速度を算出
	float speed = MOVE_SPEED * rate;

	// 速度を算出
	_move = VScale(_move, speed);

	// カメラのアングルを行列に変換
	_rotMatrix = MGetRotY(cameraAngle);

	// 移動方向を決定
	_movementDirection = VTransform(_move, _rotMatrix);
}

void PlayerMove::Draw()
{
}
