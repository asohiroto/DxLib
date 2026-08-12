#include "MagicCollision.h"
#include "Players/Player.h"
#include "Enemys/Enemy.h"
#include "Bases/MagicBase.h"
#include <DxLib.h>

MagicCollision::MagicCollision() :
	_isPlayerHit(false),
	_wasPlayerHit(false),
	_isEnemyHit(false),
	_wasEnemyHit(false),
	_hitEnemyMagicInd(-1),
	_hitPlayerMagicInd(-1)
{
}

MagicCollision::~MagicCollision()
{
}

void MagicCollision::Init()
{
}

void MagicCollision::End()
{
}

void MagicCollision::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy>pEnemy, MagicList& playerList, MagicList& enemyList)
{
	Character::CharacterData player = pPlayer->GetPlayerData();
	Character::CharacterData enemy = pEnemy->GetEnemyData();

	_hitEnemyMagicInd = PlayerHitCheck(player, enemyList);
	_hitPlayerMagicInd = EnemyHitCheck(enemy, playerList);

	_wasPlayerHit = _isPlayerHit;
	if (_hitEnemyMagicInd >= 0) _isPlayerHit = true;
	else _isPlayerHit = false;

	_wasEnemyHit = _isEnemyHit;
	if (_hitPlayerMagicInd >= 0) _isEnemyHit = true;
	else _isEnemyHit = false;

	if (IsPlayerHit())
	{
		pPlayer->SetColor(0x00ff00);
		enemyList[_hitEnemyMagicInd].isExist = false;
	}

	if (IsEnemyHit())
	{
		pEnemy->SetHit();
		playerList[_hitPlayerMagicInd].isExist = false;
	}
}

void MagicCollision::Draw()
{
}

int MagicCollision::PlayerHitCheck(const Character::CharacterData& player, const MagicList& enemyList)
{
	// 敵が撃った魔法全てで順に計算
	for (int i = 0; i < enemyList.size(); i++)
	{
		if (!enemyList[i].isExist) continue;

		float distance = 0.0f;

		if (enemyList[i].type == MagicBase::MagicType::MagicBeam)
		{
			distance = Segment_Segment_MinLength
			(player.segmentStPos, player.segmentEndPos,
				enemyList[i].segmentStPos, enemyList[i].segmentEndPos);
		}
		else
		{
			// 距離を計測
			distance = Segment_Point_MinLength(player.segmentStPos, player.segmentEndPos, enemyList[i].pos);
		}

		// めり込みの深さを計算
		float dipth = player.radius + enemyList[i].radius - distance;

		// 当たっている番号を返す
		if (dipth >= 0)
		{
			return i;
		}

	}

	return -1;
}

int MagicCollision::EnemyHitCheck(const Character::CharacterData& enemy, const MagicList& playerList)
{

	for (int i = 0; i < playerList.size(); i++)
	{
		if (!playerList[i].isExist) continue;

		float distance = Segment_Point_MinLength(enemy.segmentStPos, enemy.segmentEndPos, playerList[i].pos);

		float dipth = enemy.radius + playerList[i].radius - distance;

		if (dipth >= 0)
		{
			return i;
		}
	}

	return -1;
}