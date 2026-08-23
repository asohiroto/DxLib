#pragma once
#include "Bases/SceneBase.h"
#include<memory>

class Player;
class Camera;
class Input;
class Enemy;
class EnemyManager;
class Collision;
class MagicManager;
class MagicCollision;
class UIManager;
class SkyDome;
class EffectManager;

class SceneMain :
	public SceneBase
{
public:
	SceneMain();
	virtual ~SceneMain() override;
	void Init();
	void End() override;
	void Update(std::shared_ptr<Input> pInput) override;
	void Draw() override;
	// グリッドを描画する関数
	void DrawStage();
	// プレイヤーのHPのゲッター
	int GetPlayerHp() const;
	// エネミーのHPのゲッター
	int GetEnemyHp() const;
	// 各ハンドルのセッター
	void SetCharacterH(int playerH, int enemyH) { _playerTempH = playerH; _enemyTempH = enemyH; }
	void SetSkyDomeH(int domeH) { _domeTempH = domeH; }
	void SetMagicH(int shotH, int missileH, int hitH) { _shotTempH = shotH; _missileTempH = missileH; _hitTempH = hitH; }

private:
	// プレイヤークラスの共有ポインタ
	std::shared_ptr<Player> p_Player;
	// カメラクラスの共有ポインタ
	std::shared_ptr<Camera> p_Camera;
	// エネミー管理クラスの共有ポインタ
	std::shared_ptr<EnemyManager> p_EManager;
	// コリジョンクラスの共有ポインタ
	std::shared_ptr<Collision> p_Coll;
	// 魔法管理クラスの共有ポインタ
	std::shared_ptr<MagicManager> p_MManager;
	// 魔法のコリジョンクラスの共有ポインタ
	std::shared_ptr<MagicCollision> p_MColl;
	// UI管理クラスのポインタ
	std::shared_ptr<UIManager> p_UI;
	// スカイドームのポインタ
	std::unique_ptr<SkyDome> p_Dome;
	// エフェクトマネージャーの共有ポインタ
	std::shared_ptr<EffectManager> p_EffectManager;

	// 各クラスに渡す用のハンドル
	int _playerTempH;
	int _enemyTempH;
	int _domeTempH;
	int _shotTempH;
	int _missileTempH;
	int _hitTempH;

};
