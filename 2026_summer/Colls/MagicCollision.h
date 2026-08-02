#pragma once
#include "Bases/Character.h"
#include "Bases/MagicBase.h"
#include <memory>
#include <vector>

class Player;
class Enemy;

class MagicCollision
{
private:
	// 生成された魔法を格納するリストの型
	using MagicList = std::vector<MagicBase::MagicData>;

public:
	MagicCollision();
	virtual ~MagicCollision();
	void Init();
	void End();
	void Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy>pEnemy, MagicList& playerList, MagicList& enemyList);
	void Draw();

private:
	// プレイヤーに何番目の敵の魔法が当たったか
	int PlayerHitCheck(Character::CharacterData player, MagicList enemyList);
	// 敵に何番目のプレイヤーの魔法が当たったか
	int EnemyHitCheck(Character::CharacterData enemy, MagicList playerList);
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
	int _hitEnemyMagicInd;
	// 敵にヒットしたプレイヤーの魔法の番号
	int _hitPlayerMagicInd;
};