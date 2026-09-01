#pragma once
#include "Bases/SceneBase.h"
#include "Effects/EffectHandles.h"
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
	// ヒット時のエフェクトのハンドルを渡す
	int GetHitEffectH() const { return _hitEffectH; }
	// 大気エフェクトのハンドルを渡す
	int GetAtmosH() const { return _atmosEffectH; }
	// UIのハンドルを渡す
	int GetPlayerHpBarH() const { return _playerHpBarH; }
	int GetEnemyHpBarH() const { return _enemyHpBarH; }
	int GetUltGaugeH() const { return _ultGaugeH; }

	EffectHandles GetHandles() const { return _playerMagics; }

private:
	// モデルのハンドルたち
	int _playerH, _enemyH;
	int _domeH;
	// モデルをロードしている数
	int _totalRequestNum;
	// プレイヤーが使用する魔法のハンドル
	EffectHandles _playerMagics;
	// ヒットエフェクトのハンドル
	int _hitEffectH;
	// 大気エフェクトのハンドル
	int _atmosEffectH;
	// カウンタ
	int _count;
	// 各種UIのハンドル
	int _playerHpBarH, _enemyHpBarH, _ultGaugeH;
};
