#include "LoadScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>

namespace
{
	// 読み込む最大数
	constexpr int MAX_LOAD_NUM = 5;
	// マジックショットのエフェクトサイズ
	constexpr float SHOT_EFFECT_SIZE = 3.0f;
	// ヒット時のエフェクトサイズ
	constexpr float HIT_EFFECT_SIZE = 60.0f;
}

LoadScene::LoadScene() :
	_playerH(-1), _enemyH(-1), _domeH(-1),
	_magicShotEffectH(-1), _hitEffectH(-1),
	_totalRequestNum(MAX_LOAD_NUM),
	_sceneChange(false)
{
}

LoadScene::~LoadScene()
{
	MV1DeleteModel(_playerH);
	MV1DeleteModel(_enemyH);
	MV1DeleteModel(_domeH);
}

void LoadScene::Init()
{
	_sceneChange = false;

	// 非同期処理開始
	SetUseASyncLoadFlag(true);

	_playerH = MV1LoadModel("data/models/Player_true.mv1");
	_enemyH = MV1LoadModel("data/models/EnemyModel.mv1");
	_domeH = MV1LoadModel("data/sunny_dome.mv1");

	_magicShotEffectH = LoadEffekseerEffect("data/effects/MagicShot.efkefc", SHOT_EFFECT_SIZE);
	_hitEffectH = LoadEffekseerEffect("data/effects/hitEffe.efkefc", HIT_EFFECT_SIZE);


	// 非同期処理終了
	SetUseASyncLoadFlag(false);

	// 読み込み数を取得（ロード数を）
	_totalRequestNum = GetASyncLoadNum();
}

void LoadScene::End()
{
	MV1DeleteModel(_playerH);
	MV1DeleteModel(_enemyH);
	MV1DeleteModel(_domeH);

}

void LoadScene::Update(std::shared_ptr<Input> pInput)
{
	if (GetASyncLoadNum() == 0)
		if (pInput->IsTrigger(PAD_INPUT_1))
			_sceneChange = true;
}

void LoadScene::Draw()
{
	DrawFormatString(800, 450, 0xffffff, "%d / %d", _totalRequestNum, MAX_LOAD_NUM);
}
