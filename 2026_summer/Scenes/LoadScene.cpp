#include "LoadScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

namespace
{
	// 読み込む最大数
	constexpr int MAX_LOAD_NUM = 3;
}

LoadScene::LoadScene() :
	_playerH(-1), _enemyH(-1), _domeH(-1),
	_totalRequestNum(MAX_LOAD_NUM),
	_sceneChange(false)
{
}

LoadScene::~LoadScene()
{
}

void LoadScene::Init()
{
	_sceneChange = false;

	// 非同期処理開始
	SetUseASyncLoadFlag(true);

	_playerH = MV1LoadModel("data/PlayerModel.mv1");
	_enemyH = MV1LoadModel("data/EnemyModel.mv1");
	_domeH = MV1LoadModel("data/sunny_dome.mv1");

	// 非同期処理終了
	SetUseASyncLoadFlag(false);

	// 読み込み数を取得（ロード数を）
	_totalRequestNum = GetASyncLoadNum();
}

void LoadScene::End()
{

}

void LoadScene::Update()
{
	if (GetASyncLoadNum() == 0)
		_sceneChange = true;
}

void LoadScene::Draw()
{
	DrawFormatString(800, 450, 0xffffff, "%d / %d", _totalRequestNum, MAX_LOAD_NUM);
}
