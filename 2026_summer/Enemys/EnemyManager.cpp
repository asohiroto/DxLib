#include "EnemyManager.h"
#include "EnemyMove.h"
#include "Magics/MagicBeam.h"
#include "Magics/MagicManager.h"
#include <vector>

namespace
{
	const std::vector<Enemy::CharacterState> NORM_ROUTINE =
	{
		Enemy::CharacterState::Approach,
		Enemy::CharacterState::MoveLeft,
		Enemy::CharacterState::Beam,
		Enemy::CharacterState::MoveRight,
		Enemy::CharacterState::Beam,
		Enemy::CharacterState::MoveAway
	};

	// 各行動のアニメーション保証フレーム
	constexpr int SHOT_COUNT = 20;
	constexpr int MISSILE_COUNT = 30;
	constexpr int BEAM_COUNT = 30;
	constexpr int FURY_COUNT = 30;

	// 各行動のアニメーション補正フレーム
	constexpr int BEAM_FRAME_OFFSET = 15;
}

EnemyManager::EnemyManager() :
	p_Enemy(nullptr),
	p_Move(nullptr),
	_wasLock(false),
	_isLock(false),
	_nowRoutine(NORM_ROUTINE),
	_actionCount(0)
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init(int handle, EffectHandles enemyMagics)
{
	p_Enemy = std::make_shared<Enemy>();
	p_Enemy->Init(handle);
	p_Move = std::make_shared<EnemyMove>();
	p_Move->Init();
	p_Beam = std::make_shared<MagicBeam>();
	p_Beam->Init();
	p_Beam->SetMagicBeamH(enemyMagics.beamHandle);
}

void EnemyManager::End()
{
}

void EnemyManager::Update(VECTOR playerPos, std::shared_ptr<MagicManager> pMManager)
{
	VECTOR rota = p_Move->GetDir();
	float angle = atan2f(rota.x, rota.z);

	p_Enemy->Update(angle);
	p_Move->Update(playerPos, p_Enemy);

	_actionCount++;

	_wasLock = _isLock;
	_isLock = pMManager->IsLockOn();

	if (_isLock) p_Enemy->ChangeState(Enemy::CharacterState::HitStun);
	if (_wasLock && !_isLock) ProceedNextAction();

	if (p_Move->IsActionFinished())
		ProceedNextAction();
	else
	{
		switch (p_Enemy->GetState())
		{
		case Enemy::CharacterState::Approach:
			p_Move->Approach(p_Enemy);

			break;
		case Enemy::CharacterState::MoveAway:
			p_Move->MoveAway(p_Enemy);

			break;
		case Enemy::CharacterState::MoveLeft:
			p_Move->MoveLeft(p_Enemy);

			break;
		case Enemy::CharacterState::MoveRight:
			p_Move->MoveRight(p_Enemy);

			break;
		case Enemy::CharacterState::Shot:

			break;
		case Enemy::CharacterState::Missile:

			break;
		case Enemy::CharacterState::Beam:
			if (_actionCount == (BEAM_COUNT - BEAM_FRAME_OFFSET))
				p_Beam->GenerateBeam(p_Enemy->GetPos(), p_Move->GetDir(), true, pMManager);
			if (_actionCount >= BEAM_COUNT)
				p_Move->SetActionFinished(true);
			break;
		case Enemy::CharacterState::HitStun:

			break;
		case Enemy::CharacterState::Dead:

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
		_nowRoutine = NORM_ROUTINE;

	_actionCount = 0;
	p_Move->SetActionFinished(false);
}

