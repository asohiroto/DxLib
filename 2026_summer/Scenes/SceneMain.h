#pragma once
#include "Bases/SceneBase.h"
#include "Effects/EffectHandles.h"
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
	void Init(int score, int killCount);
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
	void SetOtherH(int hitH, int atmosH) { _hitTempH = hitH; _atmosH = atmosH; }
	void SetMagics(EffectHandles playerMagicTemp) { _playerMagicsTemp = playerMagicTemp; }

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
	int _hitTempH;
	// 大気表現のエフェクトハンドル
	int _atmosH;
	// 再生中の大気表現のハンドル
	int _atmosPlayingH;
	// 大気表現のループ再生用のカウンタ
	int _atmosCount;
	// プレイヤーに渡す用のやつ
	EffectHandles _playerMagicsTemp;
	// ヒットストップする用のカウンタ
	int _hitStopCount;
	// スコア
	int _score;
	// リポップ用のカウンタ
	int _repopCount;
	// 倒した数
	int _killCount;
};
