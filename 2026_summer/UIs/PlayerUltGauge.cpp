#include "PlayerUltGauge.h"
#include <DxLib.h>

namespace
{
	// 補間度
	constexpr float LERP_RATE = 0.15f;
	// 画面の左端からずらす幅(右端を1520に揃えたアイコン左端1270からの相対オフセット)
	constexpr int WIDTH_OFFSET = 1323;
	// 画面の上端からずらす高さ(半分サイズにした分、体力バーとの積み上げ位置を再計算)
	constexpr int HEIGHT_OFFSET = 799;
	// 必殺技ゲージの幅(元の300から半分)
	constexpr int BAR_WIDTH = 150;
	// 必殺技ゲージの太さ(元の50から半分)
	constexpr int BAR_THICKNESS = 25;
	// ゲージの上下のズレ(元の30から半分)
	constexpr int DISCRE_OFFSET = 15;
	// バーの背景色
	constexpr int BG_COLOR = 0x000000;
	// 滑らかに増えるチャージの色
	constexpr int SMOOTH_COLOR = 0xff00ff;
	// チャージ中（最大未満）の色
	constexpr int CHARGING_COLOR = 0x00ff00;
	// チャージ最大時の色
	constexpr int FULL_COLOR = 0xffff00;
	// 半分の割合
	constexpr float HALF_RATE = 0.5f;
	// アイコンの表示座標(右5%セーフゾーンに沿って右端を1520に配置、縦位置は体力バーの下のまま)
	constexpr int ICON_POS_X = 1270;
	constexpr int ICON_POS_Y = 764;
	// アイコンの表示サイズ(元画像500x182を半分に縮小)
	constexpr int ICON_WIDTH = 250;
	constexpr int ICON_HEIGHT = 91;
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
	DrawUltGauge(1.0f, BG_COLOR);
	// 滑らかに増える必殺技のチャージ
	DrawUltGauge(_dispRate, SMOOTH_COLOR);

	// 直ちに増える必殺技のチャージ
	if (_nowCharge < _maxCharge)
		DrawUltGauge(_nowRate, CHARGING_COLOR);
	else if (_nowCharge >= _maxCharge)
		DrawUltGauge(_nowRate, FULL_COLOR);

	// チャージ量が最大の半分を超えたら描画
	if (_nowCharge >= (_maxCharge * HALF_RATE))
		DrawQuadrangle(
			WIDTH_OFFSET + DISCRE_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + static_cast<int>(BAR_WIDTH * HALF_RATE) + DISCRE_OFFSET, HEIGHT_OFFSET,
			WIDTH_OFFSET + static_cast<int>(BAR_WIDTH * HALF_RATE), HEIGHT_OFFSET + BAR_THICKNESS, WIDTH_OFFSET, HEIGHT_OFFSET + BAR_THICKNESS,
			FULL_COLOR, true);

	/*DrawQuadrangle(
		WIDTH_OFFSET - 2 + (BAR_WIDTH * 0.5f), HEIGHT_OFFSET, WIDTH_OFFSET + (BAR_WIDTH * 0.5f) + 2, HEIGHT_OFFSET,
		WIDTH_OFFSET + 2 + DISCRE_OFFSET + (BAR_WIDTH * 0.5f), HEIGHT_OFFSET + BAR_THICKNESS, WIDTH_OFFSET - 2 + DISCRE_OFFSET + (BAR_WIDTH * 0.5f), HEIGHT_OFFSET + BAR_THICKNESS,
		0x000000, true
	);*/

	DrawExtendGraph(ICON_POS_X, ICON_POS_Y, ICON_POS_X + ICON_WIDTH, ICON_POS_Y + ICON_HEIGHT, _ultGaugeH, true);
}

void PlayerUltGauge::DrawUltGauge(float rate, int color)
{
	DrawQuadrangle(
		WIDTH_OFFSET + DISCRE_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + static_cast<int>(BAR_WIDTH * rate) + DISCRE_OFFSET, HEIGHT_OFFSET,
		WIDTH_OFFSET + static_cast<int>(BAR_WIDTH * rate), HEIGHT_OFFSET + BAR_THICKNESS, WIDTH_OFFSET, HEIGHT_OFFSET + BAR_THICKNESS,
		color, true);
}
