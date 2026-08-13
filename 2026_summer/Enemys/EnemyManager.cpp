#include "EnemyManager.h"
#include "Enemy.h"
#include "EnemyMove.h"
#include "Magics/MagicBeam.h"
#include "Magics/MagicManager.h"

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
	p_Beam = std::make_shared<MagicBeam>();
	p_Beam->Init();
}

void EnemyManager::End()
{
}

void EnemyManager::Update(VECTOR playerPos, std::shared_ptr<MagicManager> pMManager)
{
	VECTOR rota = p_Move->GetDir();
	float angle = atan2f(rota.x, rota.z) + DX_PI_F;

	p_Enemy->Update(angle);

	if (pMManager->IsLockOn()) p_Enemy->ChangeState(Enemy::EnemyState::HitStun);
	else p_Enemy->ChangeState(Enemy::EnemyState::Move);

	switch (p_Enemy->GetState())
	{
	case Enemy::EnemyState::Move:
		p_Move->Update(playerPos, p_Enemy);
		break;

	case Enemy::EnemyState::Attack:
		p_Beam->GenerateBeam(p_Enemy->GetPos(), p_Move->GetDir(), true, pMManager);
		p_Enemy->ChangeState(Enemy::EnemyState::Move);
		break;

	case Enemy::EnemyState::HitStun:
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

