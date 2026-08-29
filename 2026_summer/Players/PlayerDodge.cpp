#include "PlayerDodge.h"
#include "Inputs/Input.h"
#include <DxLib.h>
#include <algorithm>

namespace
{
	// 回避速度
	constexpr float SPEED = 40.0f;
	// 最大回避距離
	constexpr float MAX_DISTANCE = 600.0f;
	// 回避のクールダウンフレーム
	constexpr int DODGE_COOLDOWN = 30;
	// 画面の左端からずらす幅
	constexpr int WIDTH_OFFSET = 1370;
	// 画面の上端からずらす高さ
	constexpr int HEIGHT_OFFSET = 860;
	// クールダウンバーの幅
	constexpr int BAR_WIDTH = 200;
	// クールダウンバーの太さ
	constexpr int BAR_THICKNESS = 30;
}

PlayerDodge::PlayerDodge() :
	_dodgeDir(VGet(0.0f, 0.0f, 0.0f)),
	_isDodge(false),
	_dodgeDistance(0.0f),
	_dodgedPos(VGet(0.0f, 0.0f, 0.0f)),
	_dodgeCoolCount(60)
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
	_dodgeCoolCount++;

	if (pInput->IsTrigger(PAD_INPUT_5) && _dodgeCoolCount >= DODGE_COOLDOWN)
		_isDodge = true;

	if (_isDodge)
	{
		_dodgeDistance += SPEED;
		if (_dodgeDistance <= MAX_DISTANCE)
		{
			//_dodgedPos = VAdd(_dodgedPos, VScale(_dodgeDir, SPEED));
			_dodgedPos = VScale(_dodgeDir, SPEED);
		}
		else if (_dodgeDistance > MAX_DISTANCE)
		{
			_dodgedPos = VGet(0.0f, 0.0f, 0.0f);
			_isDodge = false;
			_dodgeDistance = 0.0f;
			_dodgeCoolCount = 0;
		}
	}
}

void PlayerDodge::Draw()
{
	DodgeCoolBar();
}

void PlayerDodge::CalDirection(std::shared_ptr<Input> pInput, float cameraAngle)
{
	// 移動の方向に代入
	int _mx = pInput->GetLeftStickX();
	int _mz = pInput->GetLeftStickY();

	// 回避方向を決定
	VECTOR input = VGet(static_cast<float>(_mx), 0.0f, static_cast<float>(-_mz));

	if (VSize(input) > 0)
		// 正規化し、方向を取得
		_dodgeDir = VNorm(input);

	// カメラのアングルを行列に変換
	MATRIX _rotMatrix = MGetRotY(cameraAngle);

	// 移動方向を決定
	_dodgeDir = VTransform(_dodgeDir, _rotMatrix);
}

void PlayerDodge::DodgeCoolBar()
{
	float rate = static_cast<float>(_dodgeCoolCount) / static_cast<float>(DODGE_COOLDOWN);
	rate = std::clamp(rate, 0.0f, 1.0f);

	DrawBox(WIDTH_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + BAR_WIDTH, HEIGHT_OFFSET + BAR_THICKNESS, 0x000000, true);
	DrawBox(WIDTH_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + (BAR_WIDTH * rate), HEIGHT_OFFSET + BAR_THICKNESS, 0xffffff, true);
}

void PlayerDodge::ResetDodgeCoolCount()
{
	_dodgeCoolCount = DODGE_COOLDOWN;
}
