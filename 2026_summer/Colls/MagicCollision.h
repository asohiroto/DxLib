#pragma once
#include "Bases/Character.h"
#include <memory>

class Player;
class Enemy;

class MagicCollision
{
public:
	MagicCollision();
	virtual ~MagicCollision();
	void Init();
	void End();
	void Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy>pEnemy);
	void Draw();

private:
	// プレイヤーに何番目の敵の魔法が当たったか
	int PlayerHitCheck(Character::CharacterData player);
	// 敵に何番目のプレイヤーの魔法が当たったか
	int EnemyHitCheck(Character::CharacterData enemy);
	// プレイヤーに当たった瞬間を取得
	bool IsPlayerHit() { return (_isPlayerHit && !_wasPlayerHit); }
	// 敵に当たった瞬間を取得
	bool IsEnemyHit() { return (_isEnemyHit && !_wasEnemyHit); }

private:
	// 今のフレームでプレイヤーに魔法がヒットしたか
	bool _isPlayerHit;
	// 前のフレームでプレイヤーに魔法がヒットしたか
	bool _wasPlayerHit;
	// 今のフレームで敵に魔法がヒットしたか
	bool _isEnemyHit;
	// 前のフレームで敵に魔法がヒットしたか
	bool _wasEnemyHit;
	// プレイヤーにヒットした敵の魔法の番号
	int _playerHitInd;
	// 敵にヒットしたプレイヤーの魔法の番号
	int _enemyHitInd;
};