#include "ExplainScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

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

void ExplainScene::Update(std::shared_ptr<Input> pInput)
{
	_count++;

	if (_count > 60)
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

	if (_count % 40 < 20)
	{
		SetFontSize(60);
		DrawFormatString(355, 625, 0xffffff, "Press X to Game Start...");
		DrawFormatString(350, 620, 0x000000, "Press X to Game Start...");
		SetFontSize(20);
	}
}
