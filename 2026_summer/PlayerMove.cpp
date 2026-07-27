#include "PlayerMove.h"
#include "Input.h"

namespace
{
	float MOVE_SPEED = 12.0f;
}

PlayerMove::PlayerMove():
	_move(VGet(0.0f, 0.0f, 0.0f))
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

void PlayerMove::Update(std::shared_ptr<Input> pInput)
{
	// 移動の方向に代入
	int mx = pInput->GetLeftStickX();
	int mz = pInput->GetLeftStickY();
	// 移動量に代入
	_move = VGet(static_cast<float>(mx), 0.0f, static_cast<float>(-mz));
	// 入力値（傾き具合）を取得
	float length = VSize(_move);
	// 0.0~1.0の間の値に変換
	float rate = length / 1000;
	// 正規化し、方向を取得
	_move = VNorm(_move);
	// 入力値を考慮した速度を算出
	float speed = MOVE_SPEED * rate;
	// 速度を算出
	_move = VScale(_move, speed);
}

void PlayerMove::Draw()
{
}
