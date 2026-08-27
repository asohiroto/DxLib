#pragma once
#include "Enemy.h"
#include <DxLib.h>
#include <memory>
#include <vector>

class Enemy;
class EnemyMove;
class MagicBeam;
class MagicManager;

class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();
	void Init(int handle, int beamH);
	void End();
	void Update(VECTOR playerPos, std::shared_ptr<MagicManager> pMManager);
	void Draw();
	// 座標のゲッター
	VECTOR GetEnemyPos() const;
	// エネミーのポインタを渡す
	std::shared_ptr<Enemy> GetEnemyPointer() const;
	// 敵の最大HPのゲッター
	float GetMaxHp() const;
	// 敵の現在HPのゲッター
	float GetNowHp() const;
	// ルーチンテーブルをせっとする
	void SetRoutine(std::vector<Enemy::CharacterState> routine) { _nowRoutine = routine; }
	// 次の行動に進める
	void ProceedNextAction();

private:
	// エネミーの共有ポインタ
	std::shared_ptr<Enemy> p_Enemy;
	// エネミーの移動状態時処理
	std::shared_ptr<EnemyMove> p_Move;
	// マジックビームの共有ポインタ
	std::shared_ptr<MagicBeam> p_Beam;
	// ロックされていたか
	bool _wasLock;
	// 今ロックされているか
	bool _isLock;
	// 今のルーチンテーブル
	std::vector<Enemy::CharacterState> _nowRoutine;
};