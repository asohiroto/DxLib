#include "LoadScene.h"
#include "SceneManager.h"
#include <DxLib.h>

LoadScene::LoadScene() :
	p_Manager(nullptr),
	_totalRequestNum(0),
	_playerModelH(-1),
	_skyDomeH(-1)
{
}

LoadScene::~LoadScene()
{
}

void LoadScene::Init(SceneManager* pManager)
{
	p_Manager = pManager;

	SetUseASyncLoadFlag(true);

	_playerModelH = MV1LoadModel("data/model_army.mv1");
	_playerSubModelH = MV1LoadModel("data/model_army.mv1");
	_skyDomeH = MV1LoadModel("data/sunny_dome.mv1");

	SetUseASyncLoadFlag(false);

	_totalRequestNum = GetASyncLoadNum();

}

void LoadScene::Update()
{
	if (GetASyncLoadNum() == 0)
		p_Manager->ChangeScene(SceneManager::SceneName::GAME, 0);
}

void LoadScene::Draw()
{
	SetFontSize(60);
	DrawString(300, 300, "Now Loading...", 0xffffff);

	// 残り件数から進捗率を計算して表示
	if (_totalRequestNum > 0)
	{
		int remain = GetASyncLoadNum();
		float rate = 1.0f - (static_cast<float>(remain) / _totalRequestNum);
		DrawBox(300, 400, 300 + static_cast<int>(600 * rate), 430, 0x00ff00, true);
	}
}