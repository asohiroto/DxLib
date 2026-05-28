#include "DxLib.h"
#include "Enemy.h"
#include"Character.h"
#include"Player.h"

Enemy::Enemy()
{
	m_pos = Vec2(0, 0);

	// m_pPlayerの初期化
	m_pPlayer = nullptr;
}

Enemy::~Enemy()
{
	DeleteGraph(units[0].handle);
}

void Enemy::Init(Player* pPlayer)
{
	m_angle = 0;
	m_pPlayer = pPlayer;

	units.clear();

	UnitData knight;
	knight.name = "桂馬";
	knight.soldierCount = 500;
	knight.attack = 30;
	knight.speed = 8.0f;
	knight.handle = LoadGraph("media/keima.png");
	knight.isEnemy = true;

	units.push_back(knight);
}

void Enemy::Update()
{
	// プレイヤーの位置を目的地としベクトルを作成
	m_targetPos = m_pPlayer->m_pos;
	Vec2 toTarget = Vec2(m_targetPos.x - m_pos.x, m_targetPos.y - m_pos.y);
	
	m_angle = atan2f(toTarget.y, toTarget.x);

	// 距離を計算
	float distance = toTarget.length();

	if (distance <= units[0].speed)
	{
		m_pos = m_targetPos;
		DrawCircle(m_targetPos.x + 42.5f, m_targetPos.y + 35.5f, 15.0f, (255, 255, 255),TRUE);
	}
	else
	{
		Vec2 direction = toTarget.getNormalize();
		m_pos += direction * units[0].speed;
	}
}

void Enemy::Draw()
{
	Character::Draw(m_pos.x, m_pos.y, m_angle, units[0].handle);
}