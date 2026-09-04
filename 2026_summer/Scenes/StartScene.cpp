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
	// デフォルトのフォントサイズ
	constexpr int NORMAL_FONT_SIZE = 20;
	// 案内文字の色
	constexpr int PROMPT_COLOR = 0xff00ff;
	// 縁取り文字の色
	constexpr int SHADOW_COLOR = 0x000000;
	// 上下に揺れるアニメーションの速さ
	constexpr float BOB_SPEED = 0.1f;
	// 「Press」の揺れ幅
	constexpr float BOB_AMPLITUDE_1 = 25.0f;
	// 「to Start...」の揺れ幅
	constexpr float BOB_AMPLITUDE_2 = 30.0f;
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

void StartScene::Update(const std::shared_ptr<Input>& pInput)
{
	_angle += ROTATE_SPEED;
	_count++;

	_startY1 = static_cast<int>(sin(_count * BOB_SPEED) * BOB_AMPLITUDE_1);
	_startY2 = static_cast<int>(sin(_count * BOB_SPEED) * BOB_AMPLITUDE_2);

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
	DrawFormatString(100 + _startY1, 750, PROMPT_COLOR, "Press");
	DrawFormatString(100 + _startY2, 750, SHADOW_COLOR, "Press");
	DrawGraph(550 + _startY1, 710, _xButtonH, true);
	DrawFormatString(770 + _startY1, 750, PROMPT_COLOR, "to Start...");
	DrawFormatString(770 + _startY2, 750, SHADOW_COLOR, "to Start...");
	SetFontSize(NORMAL_FONT_SIZE);
}
