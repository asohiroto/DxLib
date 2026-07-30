#include "Collision.h"
#include "Bases/Character.h"
#include "Players/Player.h"
#include "Enemys/Enemy.h"

Collision::Collision()
{
}

Collision::~Collision()
{
}

void Collision::Init()
{
}

void Collision::End()
{
}

void Collision::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy> pEnemy)
{
	CharacterHitCheck(pPlayer, pEnemy);
}

void Collision::Draw()
{
}

void Collision::CharacterHitCheck(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy> pEnemy)
{
	Character::CharacterData player = pPlayer->GetPlayerData();
	Character::CharacterData enemy = pEnemy->GetEnemyData();

	// 線分間の距離を取る
	float distance = Segment_Segment_MinLength(player.segmentStPos, player.segmentEndPos, enemy.segmentStPos, enemy.segmentEndPos);
	// めり込みの深さ
	float depth = player.radius + enemy.radius - distance;
	// 押し戻す方向
	VECTOR pullBackDirection = VGet(player.pos.x - enemy.pos.x, 0.0f, player.pos.z - enemy.pos.z);
	pullBackDirection = VNorm(pullBackDirection);

	if (depth >= 0)
	{
		if (player.isHit == false)pPlayer->OnHit();
		if (enemy.isHit == false)pEnemy->OnHit();

		pPlayer->SetPos(VAdd(player.pos, VScale(pullBackDirection, depth)));
	}
}
