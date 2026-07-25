#include "TitleScene.h"
#include "SceneManager.h"
#include "Input.h"
#include <DxLib.h>

TitleScene::TitleScene():
	p_Manager(nullptr)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init(SceneManager* pManager)
{
	p_Manager = pManager;
}

void TitleScene::Update(std::shared_ptr<Input> pInput)
{
	if (pInput->IsTrigger(PAD_INPUT_A))
	{
		printfDx("1\n");
		p_Manager->ChangeScene(SceneManager::SceneName::LOAD, 0);
	}
}

void TitleScene::Draw()
{
	SetFontSize(100);
	DrawString(300, 300, "スクリーンを奪え！", 0xffffff, 0xff0000);

	DrawString(300, 500, "Press A Button", 0xffffff, 0xffff00);
}