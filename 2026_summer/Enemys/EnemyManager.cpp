#include "EnemyManager.h"
#include "Enemy.h"
#include "EnemyMove.h"

EnemyManager::EnemyManager() :
	p_Enemy(nullptr),
	p_Move(nullptr)
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init()
{
	p_Enemy = std::make_shared<Enemy>();
	p_Enemy->Init();

	p_Move = std::make_shared<EnemyMove>();
	p_Move->Init();
}

void EnemyManager::End()
{
}

void EnemyManager::Update(VECTOR playerPos)
{
	p_Enemy->Update();

	switch (p_Enemy->GetState())
	{
	case Enemy::EnemyState::Move:
		p_Move->Update(playerPos, p_Enemy);
		break;

	default:
		break;
	}
}

void EnemyManager::Draw()
{
	p_Enemy->Draw();
}

VECTOR EnemyManager::GetEnemyPos() const
{
	return 	p_Enemy->GetPos();
}

std::shared_ptr<Enemy> EnemyManager::GetEnemyPointer() const
{
	return p_Enemy;
}

