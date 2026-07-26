#include "TitleScene.h"
#include "SceneManager.h"
#include "Input.h"
#include <DxLib.h>

TitleScene::TitleScene() :
	p_Manager(nullptr),
	_titlePosX(300),
	_titlePosY(300),
	_nextPosX(300),
	_nextPosY(500),
	_logoH(-1)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init(SceneManager* pManager)
{
	p_Manager = pManager;

	_logoH = LoadGraph("data/ロゴ.png");

}

void TitleScene::Update(std::shared_ptr<Input> pInput)
{
	if (pInput->IsTrigger(PAD_INPUT_A))
	{
		//printfDx("1\n");
		//printfDx("1\n");
		p_Manager->ChangeScene(SceneManager::SceneName::LOAD, 0);
	}
}

void TitleScene::Draw()
{

	// タイトル影描画
	SetFontSize(100);
	DrawString(_titlePosX + 5, _titlePosY + 5, "スクリーンを奪え！", 0x222222);

	// タイトル描画
	DrawString(_titlePosX, _titlePosY, "スクリーンを奪え！", 0xdd1100);

	// UI描画
	DrawString(_nextPosX, _nextPosY, "Press A Button", 0xffffff, 0xffff00);
}