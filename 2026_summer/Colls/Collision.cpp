#include "Collision.h"
#include "Bases/Character.h"
#include "Players/Player.h"
#include "Enemys/Enemy.h"

namespace
{
	// 接触中の当たり判定の色
	constexpr int TOUCH_COLOR = 0x00ffff;
	// 離れた瞬間の当たり判定の色
	constexpr int SEPARATE_COLOR = 0xff0000;
}

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
		pPlayer->SetColor(TOUCH_COLOR);
		pEnemy->SetColor(TOUCH_COLOR);
	}

	if (IsTouching())
	{
		pPlayer->SetPos(VAdd(player.pos, _pullBack));
	}

	if (IsSeparate())
	{
		pPlayer->SetColor(SEPARATE_COLOR);
		pEnemy->SetColor(SEPARATE_COLOR);
	}
}

void Collision::Draw()
{
}

bool Collision::CharacterHitCheck(const Character::CharacterData& player, const Character::CharacterData& enemy)
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

