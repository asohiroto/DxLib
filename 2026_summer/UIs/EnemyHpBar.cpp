#include "EnemyHpBar.h"
#include <DxLib.h>

namespace
{
	// 補間度
	constexpr float LERP_RATE = 0.15f;
	// 画面の左端からずらす幅(右端を1520に揃えたアイコン左端820からの相対オフセット)
	constexpr int WIDTH_OFFSET = 850;
	// 画面の上端からずらす高さ(半分サイズにした分の相対オフセット)
	constexpr int HEIGHT_OFFSET = 90;
	// HPバーの幅(元の1200から半分)
	constexpr int BAR_WIDTH = 600;
	// HPバーの太さ(元の70から半分)
	constexpr int BAR_THICKNESS = 35;
	// バーの背景色
	constexpr int BG_COLOR = 0x000000;
	// 滑らかに減るHPの色
	constexpr int SMOOTH_COLOR = 0xffffff;
	// 直ちに減るHPの色
	constexpr int IMMEDIATE_COLOR = 0xff0000;
	// 滑らかに減るHP(白)がアイコン枠からはみ出して見えてしまうのを隠すためのオフセット
	constexpr int SMOOTH_BAR_X_OFFSET = 5;
	// アイコンの表示座標(右5%セーフゾーンに沿って右端を1520、上5%セーフゾーンに沿って上端を45に配置)
	constexpr int ICON_POS_X = 820;
	constexpr int ICON_POS_Y = 45;
	// アイコンの表示サイズ(元画像1400x230を半分に縮小)
	constexpr int ICON_WIDTH = 700;
	constexpr int ICON_HEIGHT = 115;
}

EnemyHpBar::EnemyHpBar() :
	_nowHp(0.0f), _dispHp(0.0f), _maxHp(0.0f), _nowRate(1.0f), _dispRate(1.0f)
{
}

EnemyHpBar::~EnemyHpBar()
{
}

void EnemyHpBar::Init(float maxHp, int handle)
{
	_maxHp = maxHp;
	_nowHp = maxHp;
	_dispHp = maxHp;

	_enemyHpBarH = handle;
}

void EnemyHpBar::End()
{
}

void EnemyHpBar::Update(float nowHp)
{
	_nowHp = nowHp;
	_nowRate = nowHp / _maxHp;
	_dispHp += (_nowHp - _dispHp) * LERP_RATE;
	_dispRate = _dispHp / _maxHp;
}

void EnemyHpBar::Draw()
{
	// HPバーの最大値
	DrawHpBar(1.0f, BG_COLOR, 0);
	// 滑らかに減るHP(アイコン枠からのはみ出しを隠すため少しだけ右にずらす)
	DrawHpBar(_dispRate, SMOOTH_COLOR, SMOOTH_BAR_X_OFFSET);
	// 直ちに減るHP
	DrawHpBar(_nowRate, IMMEDIATE_COLOR, 0);

	DrawExtendGraph(ICON_POS_X, ICON_POS_Y, ICON_POS_X + ICON_WIDTH, ICON_POS_Y + ICON_HEIGHT, _enemyHpBarH, true);
}

void EnemyHpBar::DrawHpBar(float rate, int color, int xOffset)
{
	// 右端をWIDTH_OFFSET+BAR_WIDTHに固定し、HP減少分だけ左端を右へ動かすことで左から減るように見せる
	int left = WIDTH_OFFSET + xOffset + static_cast<int>(BAR_WIDTH * (1.0f - rate));
	DrawBox(left, HEIGHT_OFFSET, WIDTH_OFFSET + xOffset + BAR_WIDTH, HEIGHT_OFFSET + BAR_THICKNESS, color, true);
}
