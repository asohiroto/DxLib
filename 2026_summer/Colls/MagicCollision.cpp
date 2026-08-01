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
	_playerHitInd(-1),
	_enemyHitInd(-1)
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

void MagicCollision::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy>pEnemy)
{
	Character::CharacterData player = pPlayer->GetPlayerData();
	Character::CharacterData enemy = pEnemy->GetEnemyData();

	_playerHitInd = PlayerHitCheck(player);
	_enemyHitInd = EnemyHitCheck(enemy);

	_wasPlayerHit = _isPlayerHit;
	if (_playerHitInd >= 0) _isPlayerHit = true;
	else _isPlayerHit = false;

	_wasEnemyHit = _isEnemyHit;
	if (_enemyHitInd >= 0) _isEnemyHit = true;
	else _isEnemyHit = false;

	if (IsPlayerHit())
	{
		pPlayer->SetColor(0x00ff00);
		_playerMagic[_playerHitInd].isExist = false;
	}
}

void MagicCollision::Draw()
{
}

int MagicCollision::PlayerHitCheck(Character::CharacterData player)
{
	for (int i = 0; i < _playerMagic.size(); i++)
	{
		float distance = Segment_Point_MinLength(player.segmentStPos, player.segmentEndPos, _playerMagic[i].pos);

		float dipth = player.radius + _playerMagic[i].radius - distance;

		if (dipth >= 0)
		{
			return i;
		}
	}

	return -1;
}

int MagicCollision::EnemyHitCheck(Character::CharacterData enemy)
{
	for (int i = 0; i < _enemyMagic.size(); i++)
	{
		float distance = Segment_Point_MinLength(enemy.segmentStPos, enemy.segmentEndPos, _enemyMagic[i].pos);

		float dipth = enemy.radius + _enemyMagic[i].radius - distance;

		if (dipth >= 0)
		{
			return i;
		}
	}

	return -1;
}