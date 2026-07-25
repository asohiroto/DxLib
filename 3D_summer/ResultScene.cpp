#include "ResultScene.h"
#include "SceneManager.h"
#include "Input.h"
#include <DxLib.h>

ResultScene::ResultScene() :
	_winner(0),
	p_Manager(nullptr)
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init(SceneManager* pManager, int winner)
{
	_winner = winner;
	p_Manager = pManager;

	if (_winner == 0)
	{
		DrawString(0, 0, "ERROR!!", 0xff0000);
	}
}

void ResultScene::Update(std::shared_ptr<Input> pInput)
{
	if (pInput->IsTrigger(PAD_INPUT_B))
	{
		p_Manager->ChangeScene(SceneManager::SceneName::TITLE, 0);
	}
}

void ResultScene::Draw()
{
	DrawFormatString(300, 300, 0xffffff, "Player : %d  WIN!!", _winner);
	SetFontSize(50);
	DrawFormatString(300, 500, 0xffffff, "Press B Button");
}
