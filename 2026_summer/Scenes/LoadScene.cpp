#include "LoadScene.h"
#include "LoadScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>

namespace
{
	// 読み込む最大数
	constexpr int MAX_LOAD_NUM = 3;
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
	// フォントのサイズ
	constexpr int FONT_SIZE = 50;
}

LoadScene::LoadScene() :
	_playerH(-1), _enemyH(-1), _domeH(-1),
	_hitEffectH(-1), _atmosEffectH(-1),
	_playerMagics(),
	_totalRequestNum(MAX_LOAD_NUM),
	_count(0)
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
	_isSceneChange = false;

	// 非同期処理開始
	SetUseASyncLoadFlag(true);

	// ３Dモデルをロード
	_playerH = MV1LoadModel("data/models/Player_true.mv1");
	_enemyH = MV1LoadModel("data/models/Player_true.mv1");
	_domeH = MV1LoadModel("data/sunny_dome.mv1");

	// 非同期処理終了
	SetUseASyncLoadFlag(false);

	// プレイヤーの魔法をロード
	_playerMagics.shotHandle = LoadEffekseerEffect("data/effects/MagicShot.efkefc", SHOT_EFFECT_SIZE);
	_playerMagics.missileHandle = LoadEffekseerEffect("data/effects/MagicShot.efkefc", MISSILE_EFFECT_SIZE);
	_playerMagics.furyHandle = LoadEffekseerEffect("data/effects/MagicFury2.efkefc", FURY_EFFECT_SIZE);

	// 敵の魔法をロード
	_playerMagics.beamHandle = LoadEffekseerEffect("data/effects/MagicBeam2.efkefc", BEAM_EFFECT_SIZE);

	// その他のエフェクトをロード
	_hitEffectH = LoadEffekseerEffect("data/effects/HitEffe.efkefc", HIT_EFFECT_SIZE);
	_playerMagics.circleHandle = LoadEffekseerEffect("data/effects/MagicCircle.efkefc", CIRCLE_EFFECT_SIZE);
	_atmosEffectH = LoadEffekseerEffect("data/effects/atmo.efkefc", ATMOS_EFFECT_SIZE);

	// 読み込み数を取得（ロード数を）
	_totalRequestNum = GetASyncLoadNum();
}

void LoadScene::End()
{
	MV1DeleteModel(_playerH);
	MV1DeleteModel(_enemyH);
	MV1DeleteModel(_domeH);

	DeleteEffekseerEffect(_playerMagics.shotHandle);
	DeleteEffekseerEffect(_playerMagics.missileHandle);
	DeleteEffekseerEffect(_playerMagics.circleHandle);
	DeleteEffekseerEffect(_playerMagics.furyHandle);
	DeleteEffekseerEffect(_hitEffectH);
	DeleteEffekseerEffect(_playerMagics.beamHandle);
	DeleteEffekseerEffect(_atmosEffectH);

}

void LoadScene::Update(std::shared_ptr<Input> pInput)
{
	_count++;

	if (GetASyncLoadNum() == 0)
		_isSceneChange = true;
}

void LoadScene::Draw()
{
	int xOffset = 0;
	if (_count % 60 <= 20)
		xOffset = 0;
	else if (_count % 60 > 40)
		xOffset = 2;
	else
		xOffset = 1;

	SetFontSize(FONT_SIZE);
	DrawFormatString(700, 300, 0xffffff, "Now Loading");
	DrawFormatString(700 + (7 * FONT_SIZE) + (xOffset * FONT_SIZE), 300, 0xffffff, ".");
	SetFontSize(20);

#ifdef _DEBUG
	DrawFormatString(800, 450, 0xffffff, "%d / %d", _totalRequestNum, MAX_LOAD_NUM);
#endif
}
