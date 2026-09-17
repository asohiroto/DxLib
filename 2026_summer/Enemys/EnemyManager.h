#pragma once
#include "Enemy.h"
#include "EnemyMove.h"
#include "Magics/MagicBeam.h"
#include "Magics/MagicShot.h"
#include "Magics/MagicMissile.h"
#include "Effects/EffectHandles.h"
#include <DxLib.h>
#include <memory>
#include <vector>

class MagicManager;

class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();
	void Init(int handle, EffectHandles enemyMagics, int score);
	void End();
	void Update(VECTOR playerPos, const std::shared_ptr<MagicManager>& pMManager);
	void Draw();
	// 座標のゲッター
	VECTOR GetEnemyPos() const;
	// エネミーの参照を渡す
	Enemy& GetEnemy();
	// 敵の最大HPのゲッター
	float GetMaxHp() const;
	// 敵の現在HPのゲッター
	float GetNowHp() const;

private:
	// ルーチンテーブルをセットする
	void SetRoutine();
	// 次の行動に進める
	void ProceedNextAction();

private:
	// エネミー
	Enemy _Enemy;
	// エネミーの移動状態時処理
	EnemyMove _Move;
	// マジック
	MagicBeam _Beam;
	MagicShot _Shot;
	MagicMissile _Missile;
	// ロックされていたか
	bool _wasLock;
	// 今ロックされているか
	bool _isLock;
	// 今のルーチンテーブル
	std::vector<Enemy::CharacterState> _nowRoutine;
	// 行動のフレーム計測
	int _actionCount;
	// 距離の判定
	bool _tooNear, _tooAway;
	// 倒した数
	int _score;
};