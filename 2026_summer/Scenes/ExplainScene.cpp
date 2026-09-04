#include "ExplainScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

namespace
{
	// 操作説明を読み飛ばせるようになるまでのフレーム数
	constexpr int SKIP_WAIT_FRAME = 60;
	// 「Press X to Game Start...」の点滅周期
	constexpr int BLINK_CYCLE_FRAME = 40;
	// 点滅表示を行うフレーム数
	constexpr int BLINK_ON_FRAME = 20;
	// 案内文字のフォントサイズ
	constexpr int PROMPT_FONT_SIZE = 60;
	// デフォルトのフォントサイズ
	constexpr int NORMAL_FONT_SIZE = 20;
	// 案内文字の色
	constexpr int PROMPT_COLOR = 0xffffff;
	// 縁取り文字の色
	constexpr int SHADOW_COLOR = 0x000000;
}

ExplainScene::ExplainScene() :
	_explainH(-1),
	_canSceneChange(false),
	_count(0)
{
}

ExplainScene::~ExplainScene()
{
}

void ExplainScene::Init()
{
	_explainH = LoadGraph("data/「魔術師流自分の殺し方」操作説明.png");
	_count = 0;
}

void ExplainScene::End()
{
}

void ExplainScene::Update(const std::shared_ptr<Input>& pInput)
{
	_count++;

	if (_count > SKIP_WAIT_FRAME)
	{
		if (pInput->IsTrigger(PAD_INPUT_1))
		{
			_canSceneChange = true;
		}
	}
}

void ExplainScene::Draw()
{
	DrawGraph(0, 0, _explainH, false);

	if (_count % BLINK_CYCLE_FRAME < BLINK_ON_FRAME)
	{
		SetFontSize(PROMPT_FONT_SIZE);
		DrawFormatString(355, 625, PROMPT_COLOR, "Press X to Game Start...");
		DrawFormatString(350, 620, SHADOW_COLOR, "Press X to Game Start...");
		SetFontSize(NORMAL_FONT_SIZE);
	}
}
