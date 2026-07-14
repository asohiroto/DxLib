#include "ClearScene.h"
#include"GameDefine.h"
#include"DxLib.h"
#include"AsoDxLib/Mouse.h"
#include"AsoDxLib/Color.h"
#include"SceneManager.h"
#include"StartScene.h"
#include<memory>

using namespace GameDefine;

ClearScene::ClearScene(SceneManager& _sceneManager) :
	SceneBase(_sceneManager),
	_mousePosX(0),
	_mousePosY(0),
	_bgH(-1),
	_count(0),
	_arrow1X(0),
	_arrow2X(0),
	_clear1Y(0),
	_clear2Y(0)
{

}

ClearScene::~ClearScene()
{
	DeleteGraph(_bgH);
}

void ClearScene::Init()
{
	_bgH = LoadGraph("data/クリアシーン.png");
}

void ClearScene::End()
{

}

void ClearScene::Update()
{
	_count++;

	_arrow1X = (int)(sin(_count * 0.1) * 5);
	_arrow2X = (int)(sin((_count * 0.1) + 0.2) * 5);
	_clear1Y = (int)(cos(_count * 0.1) * 11);
	_clear2Y = (int)(cos((_count * 0.1) + 0.5) * 11);

	GetMousePoint(&_mousePosX, &_mousePosY);

	if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
	{
		_sceneManager.ChangeScene(std::make_shared<StartScene>(_sceneManager));
	}
}


void ClearScene::Draw()
{
	int x1 = (int)_arrow1X;
	int x2 = (int)_arrow2X;
	int y1 = (int)_clear1Y;
	int y2 = (int)_clear2Y;

	DrawGraph(0, 0, _bgH, true);

	SetFontSize(260);
	DrawFormatString(18, 11 + y1, color::DarkGrayColor, "Game Clear！");
	SetFontSize(250);
	DrawFormatString(40, 11 + y2 + 18, color::YellowColor, "Game Clear！");

	SetFontSize(85);
	DrawFormatString(WIDTH / 2 + 60 + 5, HEIGHT / 2 + 200 + 5, color::RedColor, "Click to Restart");
	DrawFormatString(WIDTH / 2 + 60, HEIGHT / 2 + 200, color::WhiteColor, "Click to Restart");
	DrawFormatString(WIDTH / 2 + 60 + 600 + x1, HEIGHT / 2 + 200, color::RedColor, " ⇒");
	DrawFormatString(WIDTH / 2 + 60 + 600 - 5 + x2, HEIGHT / 2 + 200 - 5, color::WhiteColor, " ⇒");
	SetFontSize(20);
}
