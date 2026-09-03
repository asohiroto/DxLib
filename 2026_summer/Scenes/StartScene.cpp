#include "StartScene.h"
#include "Inputs/Input.h"
#include "SkyDome.h"
#include <DxLib.h>
#include <cmath>

namespace
{
	// ドームの回転速度
	constexpr float ROTATE_SPEED = 0.005f;
	// フォントサイズ
	constexpr int FONT_SIZE = 120;
}

StartScene::StartScene() :
	p_Dome(nullptr),
	_logoH(-1),
	_angle(0.0f),
	_startY1(0), _startY2(0),
	_count(0),
	_xButtonH(-1)
{
}

StartScene::~StartScene()
{
}

void StartScene::Init(int domeH)
{
	p_Dome = std::make_shared<SkyDome>();
	p_Dome->Init(domeH);

	_logoH = LoadGraph("data/2026_summer_Logo.png");
	_xButtonH = LoadGraph("data/GUI/xbox_X_blue.png");

	_isSceneChange = false;
}

void StartScene::End()
{
}

void StartScene::Update(std::shared_ptr<Input> pInput)
{
	_angle += ROTATE_SPEED;
	_count++;

	_startY1 = static_cast<int>(sin(_count * 0.1f) * 25);
	_startY2 = static_cast<int>(sin(_count * 0.1f) * 30);

	p_Dome->Update();
	p_Dome->SetRotate(_angle);

	if (pInput->IsTrigger(PAD_INPUT_1))
		_isSceneChange = true;
}

void StartScene::Draw()
{
	p_Dome->Draw();
	DrawGraph(200, 50, _logoH, true);

	SetFontSize(FONT_SIZE);
	DrawFormatString(100 + _startY1, 750, 0xff00ff, "Press");
	DrawFormatString(100 + _startY2, 750, 0x000000, "Press");
	DrawGraph(550 + _startY1, 710, _xButtonH, true);
	DrawFormatString(770 + _startY1, 750, 0xff00ff, "to Start...");
	DrawFormatString(770 + _startY2, 750, 0x000000, "to Start...");
	SetFontSize(20);
}
