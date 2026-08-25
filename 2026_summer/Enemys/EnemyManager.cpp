#include "EnemyManager.h"
#include "Enemy.h"
#include "EnemyMove.h"
#include "Magics/MagicBeam.h"
#include "Magics/MagicManager.h"
#include <vector>

namespace
{
	constexpr std::vector<Enemy::EnemyState> NORM_ROUTINE =
	{
		Enemy::EnemyState::Approach,
		Enemy::EnemyState::MoveLeft,
		Enemy::EnemyState::Attack,
		Enemy::EnemyState::MoveRight,
		Enemy::EnemyState::Attack,
		Enemy::EnemyState::MoveAway
	};
}

EnemyManager::EnemyManager() :
	p_Enemy(nullptr),
	p_Move(nullptr),
	_wasLock(false),
	_isLock(false),
	_nowRoutine(NORM_ROUTINE)
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init(int handle, int beamH)
{
	p_Enemy = std::make_shared<Enemy>();
	p_Enemy->Init(handle);
	p_Move = std::make_shared<EnemyMove>();
	p_Move->Init();
	p_Beam = std::make_shared<MagicBeam>();
	p_Beam->Init();
	p_Beam->SetMagicBeamH(beamH);
}

void EnemyManager::End()
{
}

void EnemyManager::Update(VECTOR playerPos, std::shared_ptr<MagicManager> pMManager)
{
	VECTOR rota = p_Move->GetDir();
	float angle = atan2f(rota.x, rota.z) + DX_PI_F;

	p_Enemy->Update(angle);
	p_Move->Update(playerPos, p_Enemy);

	_wasLock = _isLock;
	_isLock = pMManager->IsLockOn();

	if (_isLock) p_Enemy->ChangeState(Enemy::EnemyState::HitStun);
	if (_wasLock && !_isLock) ProceedNextAction();

	if (p_Move->IsActionFinished())
	{
		ProceedNextAction();
	}
	else
	{
		switch (p_Enemy->GetState())
		{
		case Enemy::EnemyState::Approach:
			p_Move->Approach(p_Enemy);

			break;

		case Enemy::EnemyState::MoveAway:
			p_Move->MoveAway(p_Enemy);

			break;

		case Enemy::EnemyState::MoveLeft:
			p_Move->MoveLeft(p_Enemy);

			break;

		case Enemy::EnemyState::MoveRight:
			p_Move->MoveRight(p_Enemy);

			break;

		case Enemy::EnemyState::Attack:
			p_Beam->GenerateBeam(p_Enemy->GetPos(), p_Move->GetDir(), true, pMManager);
			p_Move->SetActionFinished(true);

			break;

		case Enemy::EnemyState::HitStun:

			break;

		case Enemy::EnemyState::Dead:

			break;

		default:
			break;
		}
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

float EnemyManager::GetMaxHp() const
{
	return p_Enemy->GetMaxHp();
}

float EnemyManager::GetNowHp() const
{
	return p_Enemy->GetNowHp();
}

void EnemyManager::ProceedNextAction()
{
	p_Enemy->ChangeState(_nowRoutine.front());
	_nowRoutine.erase(_nowRoutine.begin());

	if (_nowRoutine.empty())
	{
		_nowRoutine = NORM_ROUTINE;
	}
	p_Move->SetActionFinished(false);
}

