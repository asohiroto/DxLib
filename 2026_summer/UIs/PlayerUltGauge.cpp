#include "PlayerUltGauge.h"
#include <DxLib.h>

namespace
{
	// 補間度
	constexpr float LERP_RATE = 0.15f;
	// 画面の左端からずらす幅
	constexpr int WIDTH_OFFSET = 1200;
	// 画面の上端からずらす高さ
	constexpr int HEIGHT_OFFSET = 800;
	// 必殺技ゲージの幅
	constexpr int BAR_WIDTH = 300;
	// 必殺技ゲージの太さ
	constexpr int BAR_THICKNESS = 50;
	// ゲージの上下のズレ
	constexpr int DISCRE_OFFSET = 30;
}

PlayerUltGauge::PlayerUltGauge() :
	_nowCharge(0.0f), _dispCharge(0.0f), _maxCharge(0.0f),
	_nowRate(0.0f), _dispRate(0.0f)
{
}

PlayerUltGauge::~PlayerUltGauge()
{
}

void PlayerUltGauge::Init(float maxCharge, int handle)
{
	_maxCharge = maxCharge;

	_ultGaugeH = handle;
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

	// チャージ量が私大の半分を超えたら描画
	if (_nowCharge >= (_maxCharge / 2))
		DrawQuadrangle(
			WIDTH_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + (BAR_WIDTH * 0.5f), HEIGHT_OFFSET,
			WIDTH_OFFSET + DISCRE_OFFSET + (BAR_WIDTH * 0.5f), HEIGHT_OFFSET + BAR_THICKNESS, WIDTH_OFFSET + DISCRE_OFFSET, HEIGHT_OFFSET + BAR_THICKNESS,
			0xffff00, true);

	DrawQuadrangle(
		WIDTH_OFFSET - 2 + (BAR_WIDTH * 0.5f), HEIGHT_OFFSET, WIDTH_OFFSET + (BAR_WIDTH * 0.5f) + 2, HEIGHT_OFFSET,
		WIDTH_OFFSET + 2 + DISCRE_OFFSET + (BAR_WIDTH * 0.5f), HEIGHT_OFFSET + BAR_THICKNESS, WIDTH_OFFSET - 2 + DISCRE_OFFSET + (BAR_WIDTH * 0.5f), HEIGHT_OFFSET + BAR_THICKNESS,
		0x000000, true
	);

	DrawGraph(1100, 700, _ultGaugeH, true);
}

void PlayerUltGauge::DrawUltGauge(float rate, int color)
{
	DrawQuadrangle(
		WIDTH_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + (BAR_WIDTH * rate), HEIGHT_OFFSET,
		WIDTH_OFFSET + DISCRE_OFFSET + (BAR_WIDTH * rate), HEIGHT_OFFSET + BAR_THICKNESS, WIDTH_OFFSET + DISCRE_OFFSET, HEIGHT_OFFSET + BAR_THICKNESS,
		color, true);
}
