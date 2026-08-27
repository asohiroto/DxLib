#include "PlayerUltGauge.h"
#include <DxLib.h>

namespace
{
	// 補間度
	constexpr float LERP_RATE = 0.15f;
	// 画面の左端からずらす幅
	constexpr int WIDTH_OFFSET = 1270;
	// 画面の上端からずらす高さ
	constexpr int HEIGHT_OFFSET = 800;
	// 必殺技ゲージの幅
	constexpr int BAR_WIDTH = 300;
	// 必殺技ゲージの太さ
	constexpr int BAR_THICKNESS = 50;
}

PlayerUltGauge::PlayerUltGauge() :
	_nowCharge(0.0f), _dispCharge(0.0f), _maxCharge(0.0f),
	_nowRate(0.0f), _dispRate(0.0f)
{
}

PlayerUltGauge::~PlayerUltGauge()
{
}

void PlayerUltGauge::Init(float maxCharge)
{
	_maxCharge = maxCharge;
}

void PlayerUltGauge::End()
{
}

void PlayerUltGauge::Update(float nowCharge)
{
	_nowCharge = nowCharge;
	_nowRate = _nowCharge / _maxCharge;
	_dispCharge = (_nowCharge - _dispCharge) * LERP_RATE;
	_dispRate = _dispCharge / _maxCharge;
}

void PlayerUltGauge::Draw()
{
	// MPバーの最大値
	DrawUltGauge(1.0f, 0x000000);
	// 滑らかに増える必殺技のチャージ
	DrawUltGauge(_dispRate, 0xff00ff);
	// 直ちに増える必殺技のチャージ
	if (_nowCharge < _maxCharge)
		DrawUltGauge(_nowRate, 0x00ff00);
	else if (_nowCharge >= _maxCharge)
		DrawUltGauge(_nowRate, 0xffff00);
}

void PlayerUltGauge::DrawUltGauge(float rate, int color)
{
	DrawBox(WIDTH_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + (BAR_WIDTH * rate), HEIGHT_OFFSET + BAR_THICKNESS, color, true);
}
