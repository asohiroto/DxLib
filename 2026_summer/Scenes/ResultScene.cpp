#include "ResultScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

ResultScene::ResultScene() :
	_canSceneChange(false),
	_domeH(-1), _count(0.0f),
	_uiCount(0)
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init(int score, int handle)
{
	_score = score;
	_domeH = handle;
	_count = 0.0f;
	_canSceneChange = false;

	MV1SetPosition(_domeH, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(_domeH, VGet(3.0f, 3.0f, 3.0f));
}

void ResultScene::End()
{
}

void ResultScene::Update(std::shared_ptr<Input> pInput)
{
	_count += 0.01f;
	_uiCount++;

	MV1SetRotationXYZ(_domeH, VGet(0.0f, _count, 0.0f));

	if (pInput->IsTrigger(PAD_INPUT_1))
	{
		_canSceneChange = true;
	}

}

void ResultScene::Draw()
{
	MV1DrawModel(_domeH);

	SetFontSize(190);
	DrawFormatString(0, 110, 0xffff00, "Game Result");
	DrawFormatString(0, 100, 0x000000, "Game Result");
	SetFontSize(120);
	DrawFormatString(155, 455, 0xffffff, "KILL SCORE : %d!!!", _score);
	DrawFormatString(150, 450, 0x000000, "KILL SCORE : %d!!!", _score);
	if (_uiCount % 60 <= 30)
	{
		SetFontSize(100);
		DrawFormatString(105, 805, 0xff00ff, "Press X to Restart...");
		DrawFormatString(100, 800, 0x000000, "Press X to Restart...");
	}
	SetFontSize(20);
}
