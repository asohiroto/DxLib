#include "ClearScene.h"
#include"GameDefine.h"
#include"DxLib.h"
#include"AsoDxLib/Mouse.h"
#include"SceneManager.h"
#include"StartScene.h"
#include<memory>

using namespace GameDefine;

ClearScene::ClearScene(SceneManager& _sceneManager) :
	SceneBase(_sceneManager),
	_mousePosX(0),
	_mousePosY(0)
{

}

ClearScene::~ClearScene()
{

}

void ClearScene::Init()
{

}

void ClearScene::End()
{

}

void ClearScene::Update()
{
	GetMousePoint(&_mousePosX, &_mousePosY);

	if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
	{
		if (_mousePosX >= WIDTH / 2 - 150 && _mousePosY >= HEIGHT / 2 - 20 && _mousePosX <= WIDTH / 2 + 150 && _mousePosY <= HEIGHT / 2 + 20)
		{
			_sceneManager.ChangeScene(std::make_shared<StartScene>(_sceneManager));
		}
	}
}

void ClearScene::Draw()
{
	DrawBox(WIDTH / 2 - 150, HEIGHT / 2 - 20, WIDTH / 2 + 150, HEIGHT / 2 + 20, 0xffffff, true);
	DrawFormatString(WIDTH / 2 - 150 + 80, HEIGHT / 2 - 10, 0x000000, "Back To Start");
	DrawFormatString(WIDTH / 2 - 150 + 80, HEIGHT / 2 - 10 - 50, 0xffffff, "Game Clear!!");
}
