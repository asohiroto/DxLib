#include "PlayerMpBar.h"
#include <DxLib.h>

namespace
{
	// 補間度
	constexpr float LERP_RATE = 0.15f;
	// 画面の左端からずらす幅
	constexpr int WIDTH_OFFSET = 30;
	// 画面の上端からずらす高さ
	constexpr int HEIGHT_OFFSET = 860;
	// MPバーの幅
	constexpr int BAR_WIDTH = 200;
	// MPバーの太さ
	constexpr int BAR_THICKNESS = 30;
}

PlayerMpBar::PlayerMpBar() :
	_nowMp(0.0f), _dispMp(0.0f), _maxMp(0.0f), _nowRate(1.0f), _dispRate(1.0f)

{
}

PlayerMpBar::~PlayerMpBar()
{
}

void PlayerMpBar::Init(float maxMp)
{
	_nowMp = maxMp;
	_maxMp = maxMp;
	_dispMp = maxMp;
}

void PlayerMpBar::End()
{
}

void PlayerMpBar::Update(float nowMp)
{
	_nowMp = nowMp;
	_nowRate = _nowMp / _maxMp;
	_dispMp += (_nowMp - _dispMp) * LERP_RATE;
	_dispRate = _dispMp / _maxMp;
}

void PlayerMpBar::Draw()
{
	// MPバーの最大値
	DrawMpBar(1.0f, 0x000000);
	// 滑らかに減るMP
	DrawMpBar(_dispRate, 0xff0000);
	// 直ちに減るMP
	DrawMpBar(_nowRate, 0x0000ff);
}

void PlayerMpBar::DrawMpBar(float rate, int color)
{
	DrawBox(WIDTH_OFFSET, HEIGHT_OFFSET, WIDTH_OFFSET + (BAR_WIDTH * rate), HEIGHT_OFFSET + BAR_THICKNESS, color, true);
}
