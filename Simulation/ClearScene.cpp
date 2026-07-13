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
	_bgH(-1)
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
	GetMousePoint(&_mousePosX, &_mousePosY);

	if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
	{
		_sceneManager.ChangeScene(std::make_shared<StartScene>(_sceneManager));
	}
}


void ClearScene::Draw()
{
	DrawGraph(0, 0, _bgH, true);

	SetFontSize(260);
	DrawFormatString(18, 22, color::NavyColor, "Game Clear！");
	SetFontSize(250);
	DrawFormatString(40, 40, color::YellowColor, "Game Clear！");

	SetFontSize(85);
	DrawFormatString(WIDTH / 2 + 60 + 5, HEIGHT / 2 + 200 + 5, color::RedColor, "Click to Restart ⇒");
	DrawFormatString(WIDTH / 2 + 60, HEIGHT / 2 + 200, color::WhiteColor, "Click to Restart ⇒");

}
