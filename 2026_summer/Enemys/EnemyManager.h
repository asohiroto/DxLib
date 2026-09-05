#pragma once
#include "Enemy.h"
#include "Effects/EffectHandles.h"
#include <DxLib.h>
#include <memory>
#include <vector>

class Enemy;
class EnemyMove;
class MagicBeam;
class MagicShot;
class MagicMissile;
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
	// エネミーのポインタを渡す
	std::shared_ptr<Enemy> GetEnemyPointer() const;
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
	// エネミーの共有ポインタ
	std::shared_ptr<Enemy> p_Enemy;
	// エネミーの移動状態時処理
	std::shared_ptr<EnemyMove> p_Move;
	// マジックの共有ポインタ
	std::shared_ptr<MagicBeam> p_Beam;
	std::shared_ptr<MagicShot> p_Shot;
	std::shared_ptr<MagicMissile> p_Missile;
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