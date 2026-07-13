#include "GameOverScene.h"
#include"GameDefine.h"
#include"DxLib.h"
#include"AsoDxLib/Mouse.h"
#include"AsoDxLib/Color.h"
#include"SceneManager.h"
#include"StartScene.h"
#include<memory>

using namespace GameDefine;

GameOverScene::GameOverScene(SceneManager& _sceneManager) :
	SceneBase(_sceneManager),
	_mousePosX(0),
	_mousePosY(0),
	_bgH(-1)
{

}

GameOverScene::~GameOverScene()
{
	DeleteGraph(_bgH);
}

void GameOverScene::Init()
{
	_bgH = LoadGraph("data/ゲームオーバーシーン.png");
}

void GameOverScene::End()
{

}

void GameOverScene::Update()
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

void GameOverScene::Draw()
{
	DrawGraph(0, 0, _bgH, true);

	SetFontSize(250);
	DrawFormatString(30, 30, color::BlackColor, "Game Over...");
	DrawFormatString(20, 20, color::DarkRedColor, "Game Over...");

	SetFontSize(80);
	DrawFormatString(WIDTH / 2 - 70, HEIGHT / 2 - 10, color::DarkGrayColor, "Click to Restart...?");
	DrawFormatString(WIDTH / 2 - 70 + 5, HEIGHT / 2 - 10 + 5, color::NavyColor, "Click to Restart...?");
}
