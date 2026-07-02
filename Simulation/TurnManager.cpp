#include"TurnManager.h"
#include"DxLib.h"
#include"GameDefine.h"
#include"AsoDxLib/color.h"
#include<string>

using namespace GameDefine;

TurnManager::TurnManager() :
	_nowTurn(TurnState::PlayerSelectTurn),
	_turnCount(0)
{

}

TurnManager::~TurnManager()
{

}

void TurnManager::Init()
{
	_nowTurn = TurnState::PlayerSelectTurn;
	_turnCount = 1;
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

	DrawFormatString((NODE_WIDTH + 1) * NODE_SIZE, 0, color::WhiteColor, turnName.c_str(), true);
	DrawFormatString((NODE_WIDTH + 1) * NODE_SIZE, 20, color::WhiteColor, "%d ターン目", _turnCount, true);
}