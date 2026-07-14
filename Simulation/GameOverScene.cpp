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
	_bgH(-1),
	_count(0),
	_over1Y(0),
	_over2Y(0)
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
	_count++;

	_over1Y = (int)(sin(_count * 0.1) * 5);
	_over2Y = (int)(sin((_count * 0.1) + 0.2) * 5);

	GetMousePoint(&_mousePosX, &_mousePosY);

	if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
	{
		_sceneManager.ChangeScene(std::make_shared<StartScene>(_sceneManager));
	}

}

void GameOverScene::Draw()
{
	DrawGraph(0, 0, _bgH, true);

	SetFontSize(250);
	DrawFormatString(30, 30 + _over1Y, color::BlackColor, "Game Over...");
	DrawFormatString(20, 20 + _over2Y, color::DarkRedColor, "Game Over...");

	SetFontSize(80);
	DrawFormatString(WIDTH / 2 - 70, HEIGHT / 2 - 10, color::DarkGrayColor, "Click to Restart...?");
	DrawFormatString(WIDTH / 2 - 70 + 5, HEIGHT / 2 - 10 + 5, color::NavyColor, "Click to Restart...?");
}
