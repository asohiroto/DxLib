#include "GameOverScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

GameOverScene::GameOverScene()
{
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Init()
{
	_isSceneChange = false;
}

void GameOverScene::End()
{
}

void GameOverScene::Update(std::shared_ptr<Input> pInput)
{
	if (pInput->IsTrigger(PAD_INPUT_1))
		_isSceneChange = true;
}

void GameOverScene::Draw()
{
}
