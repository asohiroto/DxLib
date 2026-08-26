#include "ClearScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

ClearScene::ClearScene()
{
}

ClearScene::~ClearScene()
{
}

void ClearScene::Init()
{
	_isSceneChange = false;
}

void ClearScene::End()
{
}

void ClearScene::Update(std::shared_ptr<Input> pInput)
{
	if (pInput->IsTrigger(PAD_INPUT_1))
		_isSceneChange = true;
}

void ClearScene::Draw()
{
}
