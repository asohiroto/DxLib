#include"TurnManager.h"
#include"DxLib.h"
#include"GameDefine.h"
#include"AsoDxLib/color.h"
#include<string>

TurnManager::TurnManager() :
	_nowTurn(TurnState::PlayerSelectTurn)
{

}

TurnManager::~TurnManager()
{

}

void TurnManager::Init()
{
	_nowTurn = TurnState::PlayerSelectTurn;
}

void TurnManager::Update()
{
	
}

void TurnManager::Draw()
{
	DrawTurnState();
}

void TurnManager::TurnChange()
{
	switch (_nowTurn)
	{
	case TurnState::PlayerSelectTurn:
		_nowTurn = TurnState::SelectResultTurn;
		break;

	case TurnState::SelectResultTurn:
		_nowTurn = TurnState::EnemyTurn;
		break;

	case TurnState::EnemyTurn:
		_nowTurn = TurnState::PlayerSelectTurn;
		break;

	default:
		break;
	}
}

void TurnManager::DrawTurnState()
{
	std::string turnName;
	switch (_nowTurn)
	{
	case TurnState::PlayerSelectTurn:
		turnName = "PlayerSelectTurn";
		break;

	case TurnState::SelectResultTurn:
		turnName = "SelectResultTurn";
		break;

	case TurnState::EnemyTurn:
		turnName = "EnemyTurn";
		break;

	default:
		break;
	}

	DrawFormatString((GameDefine::NODE_WIDTH + 1) * GameDefine::NODE_SIZE, 0, color::WhiteColor, turnName.c_str(), true);
}