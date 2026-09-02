#include "ResultScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

ResultScene::ResultScene() :
	_canSceneChange(false),
	_domeH(-1), _count(0.0f)
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
	DrawFormatString(0, 0, 0x000000, "Game Result");
	SetFontSize(120);
	DrawFormatString(150, 600, 0x000000, "KILL SCORE : %d!!!", _score);
	SetFontSize(20);
}
