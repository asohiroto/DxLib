#include "StartScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

StartScene::StartScene()
{
}

StartScene::~StartScene()
{
}

void StartScene::Init()
{
	_isSceneChange = false;
}

void StartScene::End()
{
}

void StartScene::Update(std::shared_ptr<Input> pInput)
{
	if (pInput->IsTrigger(PAD_INPUT_1))
		_isSceneChange = true;
}

void StartScene::Draw()
{
}
