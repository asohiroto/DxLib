#include "PlayerHpBar.h"
#include <DxLib.h>

namespace
{
	// 補間度
	constexpr float LERP_RATE = 0.15f;
	// 画面の左端からずらす幅(アイコン画像の窓部分が始まる位置に合わせ、隠れて見えない左側の余白を削った)
	constexpr int WIDTH_OFFSET = 1241;
	// 画面の上端からずらす高さ(1.5倍サイズにした分、必殺技ゲージとの積み上げ位置を再計算)
	constexpr int HEIGHT_OFFSET = 686;
	// HPバーの幅(左側の余白を削った分だけ短縮し、右端の位置は変えていない)
	constexpr int BAR_WIDTH = 249;
	// HPバーの太さ(直前の25から1.5倍)
	constexpr int BAR_THICKNESS = 38;
	// バーの背景色
	constexpr int BG_COLOR = 0x000000;
	// 滑らかに減るHPの色
	constexpr int SMOOTH_COLOR = 0xff0000;
	// 直ちに減るHPの色
	constexpr int IMMEDIATE_COLOR = 0x00ff00;
	// アイコンの表示座標(右5%セーフゾーンに沿って右端を1520に配置、縦位置は必殺技ゲージの上のまま)
	constexpr int ICON_POS_X = 1145;
	constexpr int ICON_POS_Y = 641;
	// アイコンの表示サイズ(直前の250x82から1.5倍)
	constexpr int ICON_WIDTH = 375;
	constexpr int ICON_HEIGHT = 123;
}

PlayerHpBar::PlayerHpBar() :
	_nowHp(0.0f), _dispHp(0.0f), _maxHp(0.0f), _nowRate(1.0f), _dispRate(1.0f)
{
}

PlayerHpBar::~PlayerHpBar()
{
}

void PlayerHpBar::Init(float maxHp, int handle)
{
	_maxHp = maxHp;
	_nowHp = maxHp;
	_dispHp = maxHp;

	_playerHpBarH = handle;
}

void PlayerHpBar::End()
{
}

void PlayerHpBar::Update(float nowHp)
{
	_nowHp = nowHp;
	_nowRate = nowHp / _maxHp;
	_dispHp += (_nowHp - _dispHp) * LERP_RATE;
	_dispRate = _dispHp / _maxHp;
}

void PlayerHpBar::Draw()
{
	// HPバーの最大値
	DrawHpBar(1.0f, BG_COLOR);
	// 滑らかに減るHP
	DrawHpBar(_dispRate, SMOOTH_COLOR);
	// 直ちに減るHP
	DrawHpBar(_nowRate, IMMEDIATE_COLOR);

	DrawExtendGraph(ICON_POS_X, ICON_POS_Y, ICON_POS_X + ICON_WIDTH, ICON_POS_Y + ICON_HEIGHT, _playerHpBarH, true);
}

void PlayerHpBar::DrawHpBar(float rate, int color)
{
	DrawBox(WIDTH_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + static_cast<int>(BAR_WIDTH * rate), HEIGHT_OFFSET + BAR_THICKNESS, color, true);
}
