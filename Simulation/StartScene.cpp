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
	_scrollX(0),
	_logoH(-1),
	_logoSH(-1)
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
	_logoH = LoadGraph("data/ロゴ.png");
	_logoSH = LoadGraph("data/ロゴ影.png");
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

	DrawGraph(30, 20, _logoSH, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
	DrawGraph(x, 0, _cloudH, true);
	DrawGraph(x + _cloudWid, 0, _cloudH, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	DrawGraph(20, 10, _logoH, true);

	SetFontSize(100);
	DrawFormatString(WIDTH / 2 - 70, HEIGHT / 2 + 200 + _ctsY2, color::YellowColor, "Click To Start →");

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

}