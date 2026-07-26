#include "TitleScene.h"
#include "SceneManager.h"
#include "Input.h"
#include <DxLib.h>

TitleScene::TitleScene() :
	p_Manager(nullptr),
	_titlePosX(100),
	_titlePosY(100),
	_nextPosX(700),
	_nextPosY(500),
	_logoH(-1),
	_modelH(-1),
	_skyDomeH(-1)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init(SceneManager* pManager, int modelH, int skyDomeH)
{
	p_Manager = pManager;

	_logoH = LoadGraph("data/title.png");
	_modelH = modelH;
	_skyDomeH = skyDomeH;

	MV1SetScale(_skyDomeH, VGet(5.0f, 5.0f, 5.0f));
	MV1SetPosition(_skyDomeH, VGet(0.0f, 0.0f, 0.0f));
}

void TitleScene::Update(std::shared_ptr<Input> pInput)
{
	if (pInput->IsTrigger(PAD_INPUT_A))
	{
		//printfDx("1\n");
		//printfDx("1\n");
		p_Manager->ChangeScene(SceneManager::SceneName::GAME, 0);
	}
}

void TitleScene::Draw()
{
	MV1DrawModel(_skyDomeH);
	DrawGraph(_titlePosX, _titlePosY, _logoH, true);

	// UI描画
	DrawString(_nextPosX, _nextPosY, "Press A Button", 0xffffff, 0xffff00);
}