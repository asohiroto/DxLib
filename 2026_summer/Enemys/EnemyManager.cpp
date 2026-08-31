#include "EnemyManager.h"
#include "EnemyMove.h"
#include "EnemyRoutine.h"
#include "Magics/MagicBeam.h"
#include "Magics/MagicShot.h"
#include "Magics/MagicMissile.h"
#include "Magics/MagicManager.h"
#include <vector>

namespace
{
	// プレイヤーとエネミーの最小距離
	constexpr float MIN_DISTANCE = 1750.0f;
	// プレイヤーと敵の最大距離
	constexpr float MAX_DISTANCE = 3000.0f;
	// 各行動のアニメーション保証フレーム
	constexpr int SHOT_COUNT = 20;
	constexpr int MISSILE_COUNT = 30;
	constexpr int MAGIC_COUNT = 30;
	constexpr int FURY_COUNT = 30;
	// 各行動のアニメーション補正フレーム
	constexpr int MAGIC_FRAME_OFFSET = 15;
	// 行動ルーチンにハードルーチンを追加する体力割合
	constexpr float HARD_RATE = 0.3f;
}

EnemyManager::EnemyManager() :
	p_Enemy(nullptr),
	p_Move(nullptr),
	p_Beam(nullptr), p_Shot(nullptr), p_Missile(nullptr),
	_wasLock(false),
	_isLock(false),
	_nowRoutine(NORM_ROUTINE),
	_actionCount(0),
	_tooNear(false), _tooAway(false)
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init(int handle, EffectHandles enemyMagics, int score)
{
	p_Enemy = std::make_shared<Enemy>();
	p_Enemy->Init(handle, score);
	p_Move = std::make_shared<EnemyMove>();
	p_Move->Init();
	p_Beam = std::make_shared<MagicBeam>();
	p_Beam->Init();
	p_Shot = std::make_shared<MagicShot>();
	p_Shot->Init();
	p_Missile = std::make_shared<MagicMissile>();
	p_Missile->Init();
	p_Beam->SetMagicBeamH(enemyMagics.beamHandle);
	p_Shot->SetMagicShotH(enemyMagics.shotHandle);
	p_Missile->SetMagicMissileH(enemyMagics.missileHandle);
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

	// 距離の判定
	if (p_Move->GetDistance() <= MIN_DISTANCE) { _tooNear = true; _tooAway = false; }
	else if (p_Move->GetDistance() >= MAX_DISTANCE) { _tooNear = false; _tooAway = true; }
	else { _tooNear = false; _tooAway = false; }


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
			if (_actionCount == (MAGIC_COUNT - MAGIC_FRAME_OFFSET))
				p_Shot->GenerateShot(p_Enemy->GetPos(), p_Move->GetDir(), true, pMManager);
			if (_actionCount >= MAGIC_COUNT)
				p_Move->SetActionFinished(true);

			break;
		case Enemy::CharacterState::Missile:
			if (_actionCount == (MAGIC_COUNT - MAGIC_FRAME_OFFSET))
				p_Missile->GenerateMissile(p_Enemy->GetPos(), p_Move->GetDir(), true, pMManager);
			if (_actionCount >= MAGIC_COUNT)
				p_Move->SetActionFinished(true);

			break;
		case Enemy::CharacterState::Beam:
			if (_actionCount == (MAGIC_COUNT - MAGIC_FRAME_OFFSET))
				p_Beam->GenerateBeam(p_Enemy->GetPos(), p_Move->GetDir(), true, pMManager);
			if (_actionCount >= MAGIC_COUNT)
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

void EnemyManager::SetRoutine()
{
	if (p_Enemy->GetNowHp() <= p_Enemy->GetMaxHp() * HARD_RATE && _nowRoutine != HARD_ROUTINE)
	{
		_nowRoutine = HARD_ROUTINE;
		return;
	}
	else if (_tooNear && _nowRoutine != NEAR_ROUTINE)
	{
		_nowRoutine = NEAR_ROUTINE;
		return;
	}
	else if (_tooAway && _nowRoutine != AWAY_ROUTINE)
	{
		_nowRoutine = AWAY_ROUTINE;
		return;
	}
	else
	{
		_nowRoutine = NORM_ROUTINE;
		return;
	}
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

