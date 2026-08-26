#pragma once
#include "Bases/SceneBase.h"
#include <memory>

class Input;

class LoadScene : public SceneBase
{
public:
	LoadScene();
	virtual ~LoadScene() override;
	void Init() override;
	void End() override;
	void Update(std::shared_ptr<Input> pInput) override;
	void Draw() override;
	// プレイヤーのモデルのハンドルを渡す
	int GetPlayerH()const { return _playerH; }
	// エネミーのモデルのハンドルを渡す
	int GetEnemyH() const { return _enemyH; }
	// スカイドームのモデルのハンドルを渡す
	int GetDomeH() const { return _domeH; }
	// マジックショットのハンドルを渡す
	int GetMagicShotH()const { return _magicShotEffectH; }
	// マジックミサイルのハンドルを渡す
	int GetMissileH() const { return _magicMissileEffectH; }
	// マジックフューリーのハンドルを渡す
	int GetFuryH() const { return _magicFuryEffectH; }
	// ヒット時のエフェクトのハンドルを渡す
	int GetHitEffectH() const { return _hitEffectH; }
	// マジックサークルのハンドルを渡す
	int GetCircleEffectH() const { return _magicCircleEffectH; }
	// マジックビームのハンドルを渡す
	int GetBeamH() const { return _magicBeamEffectH; }
	// 大気エフェクトのハンドルを渡す
	int GetAtmosH() const { return _atmosEffectH; }

private:
	// モデルのハンドルたち
	int _playerH, _enemyH;
	int _domeH;
	// モデルをロードしている数
	int _totalRequestNum;
	// マジックショットのエフェクトのハンドル
	int _magicShotEffectH;
	// マジックミサイルのエフェクトのハンドル
	int _magicMissileEffectH;
	// マジックフューリーのエフェクトのハンドル
	int _magicFuryEffectH;
	// マジックサークルのエフェクトのハンドル
	int _magicCircleEffectH;
	// ヒットエフェクトのハンドル
	int _hitEffectH;
	// マジックビームのエフェクトのハンドル
	int _magicBeamEffectH;
	// 大気エフェクトのハンドル
	int _atmosEffectH;
};
