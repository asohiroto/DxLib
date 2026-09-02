#include "PlayerHpBar.h"
#include <DxLib.h>

namespace
{
	// 補間度
	constexpr float LERP_RATE = 0.15f;
	// 画面の左端からずらす幅
	constexpr int WIDTH_OFFSET = 80;
	// 画面の上端からずらす高さ
	constexpr int HEIGHT_OFFSET = 760;
	// HPバーの幅
	constexpr int BAR_WIDTH = 380;
	// HPバーの太さ
	constexpr int BAR_THICKNESS = 50;
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
	DrawHpBar(1.0f, 0x000000);
	// 滑らかに減るHP
	DrawHpBar(_dispRate, 0xff0000);
	// 直ちに減るHP
	DrawHpBar(_nowRate, 0x00ff00);

	DrawGraph(0, 700, _playerHpBarH, true);
}

void PlayerHpBar::DrawHpBar(float rate, int color)
{
	DrawBox(WIDTH_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + (BAR_WIDTH * rate), HEIGHT_OFFSET + BAR_THICKNESS, color, true);
}
