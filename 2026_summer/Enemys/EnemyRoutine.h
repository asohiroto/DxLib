#pragma once
#include "Enemy.h"
#include <vector>

// Enemy::CharacterStateのエイリアス
using EnemyState = Enemy::CharacterState;

// 通常時の行動ルーチン
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

// プレイヤーから離れすぎている時の行動ルーチン
inline const std::vector AWAY_ROUTINE =
{
	EnemyState::Approach,
	EnemyState::Shot,
	EnemyState::Approach,
	EnemyState::MoveLeft
};

// プレイヤーに近すぎる時の行動ルーチン
inline const std::vector NEAR_ROUTINE =
{
	EnemyState::MoveAway,
	EnemyState::Missile,
	EnemyState::MoveAway,
	EnemyState::Beam,
	EnemyState::MoveLeft
};

// HPが低下した際に切り替わるハードな行動ルーチン
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