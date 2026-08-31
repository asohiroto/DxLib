#pragma once
#include "Enemy.h"
#include <vector>

using EnemyState = Enemy::CharacterState;

inline constexpr std::vector NORM_ROUTINE =
{
	EnemyState::Approach,
	EnemyState::MoveLeft,
	EnemyState::Beam,
	EnemyState::MoveRight,
	EnemyState::Beam,
	EnemyState::MoveAway
};