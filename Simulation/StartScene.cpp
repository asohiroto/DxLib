#include"StartScene.h"
#include"DxLib.h"
#include"GameDefine.h"
#include"AsoDxLib/Mouse.h"
#include"MainScene.h"
#include"ClearScene.h"
#include"GameOverScene.h"
#include"SceneManager.h"

using namespace GameDefine;

StartScene::StartScene(SceneManager& _sceneManager) :
	SceneBase(_sceneManager),
	_mousePosX(0),
	_mousePosY(0)
{

}

StartScene::~StartScene()
{

}

void StartScene::Init()
{

}

void StartScene::End()
{

}

void StartScene::Update()
{
	GetMousePoint(&_mousePosX, &_mousePosY);

	if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
	{
		if (_mousePosX >= WIDTH / 2 - 150 && _mousePosY >= HEIGHT / 2 - 20 && _mousePosX <= WIDTH / 2 + 150 && _mousePosY <= HEIGHT / 2 + 20)
		{
			_sceneManager.ChangeScene(std::make_shared<MainScene>(_sceneManager));
		}
	}
}

void StartScene::Draw()
{
	DrawBox(WIDTH / 2 - 150, HEIGHT / 2 - 20, WIDTH / 2 + 150, HEIGHT / 2 + 20, 0xffffff, true);
	DrawFormatString(WIDTH / 2 - 150 + 80, HEIGHT / 2 - 10, 0x000000, "Click To Start");
}