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

void Collision::Update()
{
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

	VECTOR direction = VSub(player.pos, enemy.pos);
}
