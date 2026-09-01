#include "ResultScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

ResultScene::ResultScene() :
	_canSceneChange(false)
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init(int score)
{
	_score = score;
}

void ResultScene::End()
{
}

void ResultScene::Update(std::shared_ptr<Input> pInput)
{
	if (pInput->IsTrigger(PAD_INPUT_1))
	{
		_canSceneChange = true;
	}

}

void ResultScene::Draw()
{
	SetFontSize(120);
	DrawFormatString(100, 400, 0xffffff, "SCORE : %d !!!", _score);
	SetFontSize(20);
}
