#include "ExplainScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

ExplainScene::ExplainScene() :
	_explainH(-1)
{
}

ExplainScene::~ExplainScene()
{
}

void ExplainScene::Init()
{
	_explainH = LoadGraph("data/「魔術師流自分の殺し方」操作説明.png");
}

void ExplainScene::End()
{
}

void ExplainScene::Update(std::shared_ptr<Input> pInput)
{
	if (pInput->IsTrigger(PAD_INPUT_1))
	{
		_canSceneChange = true;
	}
}

void ExplainScene::Draw()
{
	DrawGraph(0, 0, _explainH, false);
}
