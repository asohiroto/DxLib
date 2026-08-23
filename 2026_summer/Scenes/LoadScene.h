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
	// ヒット時のエフェクトのハンドルを渡す
	int GetHitEffectH() const { return _hitEffectH; }
	// シーン遷移を行うか
	bool CanSceneChange() const { return _sceneChange; }

private:
	// モデルのハンドルたち
	int _playerH, _enemyH;
	int _domeH;
	// モデルをロードしている数
	int _totalRequestNum;
	// シーンの変更を行うか
	bool _sceneChange;
	// マジックショットのエフェクトのハンドル
	int _magicShotEffectH;
	// ヒットエフェクトのハンドル
	int _hitEffectH;

};
