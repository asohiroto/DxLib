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
		turnName = "PlayerSelectPhase";
		break;

	case TurnState::SelectResultTurn:
		turnName = "SelectResultPhase";
		break;

	case TurnState::EnemyTurn:
		turnName = "EnemyPhase";
		break;

	default:
		break;
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 122);
	DrawBox(0, 0, 64 * NODE_SIZE, 75, color::WhiteColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	SetFontSize(50);
	DrawFormatString(11 * NODE_SIZE, 0, color::BlackColor, turnName.c_str(), true);
	DrawFormatString(0, 0, color::BlackColor, "Turn %d", _turnCount, true);
	SetFontSize(20);
}