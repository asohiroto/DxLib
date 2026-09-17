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
	constexpr float MIN_DISTANCE = 1500.0f;
	// プレイヤーと敵の最大距離
	constexpr float MAX_DISTANCE = 2000.0f;
	// 各行動のアニメーション保証フレーム
	constexpr int SHOT_COUNT = 20;
	constexpr int MISSILE_COUNT = 30;
	constexpr int MAGIC_COUNT = 30;
	constexpr int FURY_COUNT = 30;
	// 各行動のアニメーション補正フレーム
	constexpr int MAGIC_FRAME_OFFSET = 15;
	// 行動ルーチンにハードルーチンを追加する体力割合
	constexpr float HARD_RATE = 0.3f;
	// ハードルーチンに切り替わるスコアの下限
	constexpr int HARD_ROUTINE_SCORE = 10;
	// 近距離・遠距離ルーチンに切り替わるスコアの下限
	constexpr int SPECIAL_ROUTINE_SCORE = 3;
}

EnemyManager::EnemyManager() :
	_wasLock(false),
	_isLock(false),
	_nowRoutine(NORM_ROUTINE),
	_actionCount(0),
	_tooNear(false), _tooAway(false), _score(0)
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init(int handle, EffectHandles enemyMagics, int score)
{
	_Enemy.Init(handle, score);
	_Move.Init();
	_Beam.Init();
	_Shot.Init();
	_Missile.Init();
	_Beam.SetMagicBeamH(enemyMagics.beamHandle);
	_Shot.SetMagicShotH(enemyMagics.shotHandle);
	_Missile.SetMagicMissileH(enemyMagics.missileHandle);

	_score = score;
}

void EnemyManager::End()
{
}

void EnemyManager::Update(VECTOR playerPos, const std::shared_ptr<MagicManager>& pMManager)
{
	// 進行方向ベクトルから向きの角度を算出
	VECTOR rota = _Move.GetDir();
	float angle = atan2f(rota.x, rota.z);

	_Enemy.Update(angle);
	_Move.Update(playerPos, _Enemy);

	_actionCount++;

	// 前フレームのロック状態を保存
	_wasLock = _isLock;
	_isLock = pMManager->IsLockOn();

	// 距離の判定
	if (_Move.GetDistance() <= MIN_DISTANCE) { _tooNear = true; _tooAway = false; }
	else if (_Move.GetDistance() >= MAX_DISTANCE) { _tooNear = false; _tooAway = true; }
	else { _tooNear = false; _tooAway = false; }


	// ロックオン中は敵を硬直させる
	if (_isLock) _Enemy.ChangeState(Enemy::CharacterState::HitStun);
	// ロックが解除された瞬間に次の行動へ進める
	if (_wasLock && !_isLock) ProceedNextAction();

	if (_Move.IsActionFinished())
		ProceedNextAction();
	else
	{
		switch (_Enemy.GetState())
		{
		case Enemy::CharacterState::Approach:
			_Move.Approach(_Enemy);

			break;
		case Enemy::CharacterState::MoveAway:
			_Move.MoveAway(_Enemy);

			break;
		case Enemy::CharacterState::MoveLeft:
			_Move.MoveLeft(_Enemy);

			break;
		case Enemy::CharacterState::MoveRight:
			_Move.MoveRight(_Enemy);

			break;
		case Enemy::CharacterState::Shot:
			// モーションに合わせたタイミングで発射
			if (_actionCount == (MAGIC_COUNT - MAGIC_FRAME_OFFSET))
				_Shot.GenerateShot(_Enemy.GetPos(), _Move.GetDir(), true, pMManager);
			// 行動時間経過で終了
			if (_actionCount >= MAGIC_COUNT)
				_Move.SetActionFinished(true);

			break;
		case Enemy::CharacterState::Missile:
			// モーションに合わせたタイミングで発射
			if (_actionCount == (MAGIC_COUNT - MAGIC_FRAME_OFFSET))
				_Missile.GenerateMissile(_Enemy.GetPos(), _Move.GetDir(), true, pMManager);
			// 行動時間経過で終了
			if (_actionCount >= MAGIC_COUNT)
				_Move.SetActionFinished(true);

			break;
		case Enemy::CharacterState::Beam:
			// モーションに合わせたタイミングで発射
			if (_actionCount == (MAGIC_COUNT - MAGIC_FRAME_OFFSET))
				_Beam.GenerateBeam(_Enemy.GetPos(), _Move.GetDir(), true, pMManager);
			// 行動時間経過で終了
			if (_actionCount >= MAGIC_COUNT)
				_Move.SetActionFinished(true);
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
	_Enemy.Draw();
}

VECTOR EnemyManager::GetEnemyPos() const
{
	return 	_Enemy.GetPos();
}

Enemy& EnemyManager::GetEnemy()
{
	return _Enemy;
}

float EnemyManager::GetMaxHp() const
{
	return _Enemy.GetMaxHp();
}

float EnemyManager::GetNowHp() const
{
	return _Enemy.GetNowHp();
}

void EnemyManager::SetRoutine()
{
	// HPが低下し、かつスコア条件を満たせばハードルーチンへ
	if (_Enemy.GetNowHp() <= _Enemy.GetMaxHp() * HARD_RATE && _nowRoutine != HARD_ROUTINE && _score > HARD_ROUTINE_SCORE)
	{
		_nowRoutine = HARD_ROUTINE;
		return;
	}
	// プレイヤーに近すぎる場合は近距離ルーチンへ
	else if (_tooNear && _nowRoutine != NEAR_ROUTINE && _score > SPECIAL_ROUTINE_SCORE)
	{
		_nowRoutine = NEAR_ROUTINE;
		return;
	}
	// プレイヤーから離れすぎている場合は遠距離ルーチンへ
	else if (_tooAway && _nowRoutine != AWAY_ROUTINE && _score > SPECIAL_ROUTINE_SCORE)
	{
		_nowRoutine = AWAY_ROUTINE;
		return;
	}
	// それ以外は通常ルーチンへ
	else
	{
		_nowRoutine = NORM_ROUTINE;
		return;
	}
}

void EnemyManager::ProceedNextAction()
{
	// ルーチンの先頭行動を実行し、リストから取り除く
	_Enemy.ChangeState(_nowRoutine.front());
	_nowRoutine.erase(_nowRoutine.begin());

	// ルーチンを全て消化したら次のルーチンを選択
	if (_nowRoutine.empty())
		SetRoutine();

	_actionCount = 0;
	_Move.SetActionFinished(false);
}

