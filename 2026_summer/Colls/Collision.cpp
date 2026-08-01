#include "Collision.h"
#include "Bases/Character.h"
#include "Players/Player.h"
#include "Enemys/Enemy.h"

Collision::Collision() :
	_isTouchNow(false),
	_isTouchLast(false)
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
	Character::CharacterData player = pPlayer->GetPlayerData();
	Character::CharacterData enemy = pEnemy->GetEnemyData();

	_isTouchLast = _isTouchNow;
	_isTouchNow = CharacterHitCheck(player, enemy);

	if (IsTouch())
	{
		if (player.isHit == false)pPlayer->SetHit(true);
		if (enemy.isHit == false)pEnemy->SetHit(true);

		pPlayer->SetColor(0x00ffff);
		pEnemy->SetColor(0x00ffff);
	}

	if (IsTouching())
	{
		pPlayer->SetPos(VAdd(player.pos, _pullBack));
	}

	if (IsSeparate())
	{
		if (player.isHit == true)pPlayer->SetHit(false);
		if (enemy.isHit == true)pEnemy->SetHit(false);

		pPlayer->SetColor(0xff0000);
		pEnemy->SetColor(0xff0000);
	}
}

void Collision::Draw()
{
}

bool Collision::CharacterHitCheck(Character::CharacterData player, Character::CharacterData enemy)
{
	// 線分間の距離を取る
	float distance = Segment_Segment_MinLength(player.segmentStPos, player.segmentEndPos, enemy.segmentStPos, enemy.segmentEndPos);
	// めり込みの深さ
	float depth = player.radius + enemy.radius - distance;
	// 押し戻す方向
	VECTOR pullBackDirection = VGet(player.pos.x - enemy.pos.x, 0.0f, player.pos.z - enemy.pos.z);
	pullBackDirection = VNorm(pullBackDirection);

	if (depth >= 0)
	{
		SetPullBack(VScale(pullBackDirection, depth));
		return true;
	}

	return false;
}

