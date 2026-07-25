#include "TitleScene.h"
#include "SceneManager.h"
#include <DxLib.h>

TitleScene::TitleScene():
	p_Manager(nullptr)
{
}

TitleScene::~TitleScene()
{
	delete p_Manager;
}

void TitleScene::Init(SceneManager* pManager)
{
	p_Manager = pManager;
}

void TitleScene::Update()
{
	if (GetJoypadInputState(DX_INPUT_PAD1) & PAD_INPUT_1)
	{
		printfDx("1\n");
		p_Manager->ChangeScene(SceneManager::SceneName::GAME);
	}
}

void TitleScene::Draw()
{
	SetFontSize(100);
	DrawString(300, 300, "カメラを奪え！", 0xffffff, 0xff0000);

	DrawString(300, 500, "Press Any Key", 0xffffff, 0xffff00);
}