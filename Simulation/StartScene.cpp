#include"StartScene.h"
#include"DxLib.h"
#include"GameDefine.h"
#include"AsoDxLib/Mouse.h"
#include"AsoDxLib/Color.h"
#include"MainScene.h"
#include"ClearScene.h"
#include"GameOverScene.h"
#include"SceneManager.h"

using namespace GameDefine;

StartScene::StartScene(SceneManager& _sceneManager) :
	SceneBase(_sceneManager),
	_mousePosX(0),
	_mousePosY(0),
	_bgH(-1),
	_fadeInAlpha(0),
	_startCount(0),
	_ctsY1(0),
	_ctsY2(0),
	_cloudH(-1),
	_cloudWid(0),
	_scrollX(0)
{

}

StartScene::~StartScene()
{
	DeleteGraph(_bgH);
	DeleteGraph(_cloudH);
}

void StartScene::Init()
{
	_bgH = LoadGraph("data/タイトル.png");
	_cloudH = LoadGraph("data/Cloud2.png");
	_cloudWid = 1613;
	_scrollX = 0;
}

void StartScene::End()
{

}

void StartScene::Update()
{
	_startCount++;

	_scrollX -= 2.0f;

	if (_scrollX <= -_cloudWid)
	{
		_scrollX += _cloudWid;
	}

	_ctsY1 = (int)(sin(_startCount * 0.1) * 10);
	_ctsY2 = (int)(sin((_startCount * 0.1) + 0.2) * 12);

	if (_startCount <= 85)
	{
		_fadeInAlpha = _startCount * 3;
	}

	GetMousePoint(&_mousePosX, &_mousePosY);

	if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
	{
		_sceneManager.ChangeScene(std::make_shared<MainScene>(_sceneManager));
	}
}

void StartScene::Draw()
{

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, _fadeInAlpha);
	DrawBox(0, 0, WIDTH, HEIGHT, 0x000000, true);

	DrawGraph(0, 0, _bgH, true);

	int x = (int)_scrollX;

	SetFontSize(250);
	DrawFormatString(20, 20, color::DarkGrayColor, "- 攻めろ！-");
	SetFontSize(100);
	DrawFormatString(WIDTH / 2 - 70 + 5, HEIGHT / 2 + 200 + 5 + _ctsY1, color::BlackColor, "Click To Start →");
	SetFontSize(20);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
	DrawGraph(x, 0, _cloudH, true);
	DrawGraph(x + _cloudWid, 0, _cloudH, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	SetFontSize(100);
	DrawFormatString(WIDTH / 2 - 70, HEIGHT / 2 + 200 + _ctsY2, color::YellowColor, "Click To Start →");

	SetFontSize(250);
	DrawFormatString(30, 30, color::CyanColor, "- 攻めろ！-");
	SetFontSize(20);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

}