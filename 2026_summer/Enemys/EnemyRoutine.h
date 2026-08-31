#pragma once
#include "Enemy.h"
#include <vector>

using EnemyState = Enemy::CharacterState;

inline const std::vector NORM_ROUTINE =
{
	EnemyState::Approach,
	EnemyState::MoveLeft,
	EnemyState::Shot,
	EnemyState::MoveRight,
	EnemyState::Missile,
	EnemyState::MoveAway,
	EnemyState::MoveLeft,
	EnemyState::Beam,
	EnemyState::MoveAway
};

inline const std::vector AWAY_ROUTINE =
{
	EnemyState::Approach,
	EnemyState::Shot,
	EnemyState::Approach,
	EnemyState::MoveLeft
};

inline const std::vector NEAR_ROUTINE =
{
	EnemyState::MoveAway,
	EnemyState::Missile,
	EnemyState::MoveAway,
	EnemyState::Beam,
	EnemyState::MoveLeft
};

inline const std::vector HARD_ROUTINE =
{
	EnemyState::Approach,
	EnemyState::Beam,
	EnemyState::MoveRight,
	EnemyState::Beam,
	EnemyState::MoveRight,
	EnemyState::Shot,
	EnemyState::Approach
};