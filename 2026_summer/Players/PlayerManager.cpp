#include "PlayerManager.h"
#include "Player.h"
#include "PlayerDodge.h"
#include "PlayerMove.h"

PlayerManager::PlayerManager() :
	p_Player(nullptr),
	p_Dodge(nullptr),
	p_Move(nullptr),
	_actionCount(0)
{
}

PlayerManager::~PlayerManager()
{
}

void PlayerManager::Init(int handle, EffectHandles playerMagics)
{
	p_Player = std::make_shared<Player>();
	p_Player->Init(handle, playerMagics);

}

void PlayerManager::End()
{
}

void PlayerManager::Update()
{
	switch (p_Player->GetNowState())
	{
	case Player::CharacterState::Approach:

		break;
	case Player::CharacterState::MoveAway:

		break;
	case Player::CharacterState::MoveLeft:

		break;
	case Player::CharacterState::MoveRight:

		break;
	case Player::CharacterState::DodgeLeft:

		break;
	case Player::CharacterState::DodgeRight:

		break;
	case Player::CharacterState::Shot:

		break;
	case Player::CharacterState::Missile:

		break;
	case Player::CharacterState::Beam:

		break;
	case Player::CharacterState::HitStun:

		break;
	case Player::CharacterState::Dead:

		break;
	default:
		break;
	}

}

void PlayerManager::Draw()
{
}
