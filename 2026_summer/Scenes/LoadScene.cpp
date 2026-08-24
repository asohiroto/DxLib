#include "LoadScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>

namespace
{
	// 読み込む最大数
	constexpr int MAX_LOAD_NUM = 5;
	// マジックショットのエフェクトサイズ
	constexpr float SHOT_EFFECT_SIZE = 90.0f;
	// マジックミサイルのエフェクトサイズ
	constexpr float MISSILE_EFFECT_SIZE = 90.0f;
	// マジックフューリーのエフェクトサイズ
	constexpr float FURY_EFFECT_SIZE = 180.0f;
	// ヒット時のエフェクトサイズ
	constexpr float HIT_EFFECT_SIZE = 180.0f;
	// マジックサークルのエフェクトサイズ
	constexpr float CIRCLE_EFFECT_SIZE = 120.0f;
	// マジックビームのエフェクトサイズ
	constexpr float BEAM_EFFECT_SIZE = 180.0f;
	// 大気のエフェクトのサイズ
	constexpr float ATMOS_EFFECT_SIZE = 240.0f;
}

LoadScene::LoadScene() :
	_playerH(-1), _enemyH(-1), _domeH(-1),
	_magicShotEffectH(-1), _magicMissileEffectH(-1), _magicFuryEffectH(-1),
	_magicBeamH(-1),
	_hitEffectH(-1), _magicCircleEffectH(-1), _atmosEffectH(-1),
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

	// ３Dモデルをロード
	_playerH = MV1LoadModel("data/models/Player_true.mv1");
	_enemyH = MV1LoadModel("data/models/EnemyModel.mv1");
	_domeH = MV1LoadModel("data/sunny_dome.mv1");

	// プレイヤーの魔法をロード
	_magicShotEffectH = LoadEffekseerEffect("data/effects/MagicShot.efkefc", SHOT_EFFECT_SIZE);
	_magicMissileEffectH = LoadEffekseerEffect("data/effects/MagicShot.efkefc", MISSILE_EFFECT_SIZE);
	_magicFuryEffectH = LoadEffekseerEffect("data/effects/MagicFury2.efkefc", FURY_EFFECT_SIZE);

	// 敵の魔法をロード
	_magicBeamH = LoadEffekseerEffect("data/effects/MagicBeam2.efkefc", BEAM_EFFECT_SIZE);

	// その他のエフェクトをロード
	_hitEffectH = LoadEffekseerEffect("data/effects/HitEffe.efkefc", HIT_EFFECT_SIZE);
	_magicCircleEffectH = LoadEffekseerEffect("data/effects/MagicCircle.efkefc", CIRCLE_EFFECT_SIZE);
	_atmosEffectH = LoadEffekseerEffect("data/effects/atmo.efkefc", ATMOS_EFFECT_SIZE);

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
