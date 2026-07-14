#include "BaseManager.h"
#include"GameDefine.h"
#include"RouteSearch.h"
#include"UIManager.h"
#include"UnitManager.h"
#include"AsoDxLib/Mouse.h"
#include"TurnManager.h"

using namespace GameDefine;

BaseManager::BaseManager() :
	_mousePosX(0),
	_mousePosY(0),
	_tileTemp(),
	_actionFlag(false),
	_unitTemp()
{

}

BaseManager::~BaseManager()
{

}

void BaseManager::Init()
{
	_mousePosX = 0;
	_mousePosY = 0;
	_actionFlag = false;

}

void BaseManager::Update(RouteSearch* rs, UIManager* um, UnitManager* unm, TurnManager* tm)
{
	GetMousePoint(&_mousePosX, &_mousePosY);

	if (tm->GetNowTurn() == TurnManager::TurnState::PlayerSelectTurn)
	{
		if (_isEneSpawn)
		{
			_isEneSpawn = false;
		}

		if (!_isCounted)
		{
			_reSpawnCount++;
			_isCounted = true;
		}
	}

	if (tm->GetNowTurn() == TurnManager::TurnState::SelectResultTurn)
	{
		if (_isCounted)
		{
			_isCounted = false;
		}
	}

	for (auto unit : _unitTemp)
	{
		unit->moveTimer++;
	}

	if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
	{
		if (_mousePosX <= NODE_WIDTH * NODE_SIZE && _mousePosY <= NODE_HEIGHT * NODE_SIZE)
		{
			Vec2 _mouseNodePos = um->ChangePixelToIndex(Vec2(_mousePosX, _mousePosY));
			_tileTemp = rs->GetNodeData(_mouseNodePos.x, _mouseNodePos.y);

			if (_tileTemp == TileType::MyBase && _reSpawnCount >= SPAWN_SPAN)
			{
				_actionFlag = true;
			}
		}
	}

	if (_actionFlag && !um->IsTargetSet())
	{
		if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
		{
			if (_mousePosX >= ACTION_X && _mousePosY >= ACTION_Y && _mousePosX <= ACTION_X + ACTION_WIDTH && _mousePosY <= ACTION_Y + (ACTION_HEIGHT / 3))
			{
				_unitTemp.push_back(SpawnUnit(UnitType::Soldier, rs));
				unm->_unitList.push_back(_unitTemp.back());
				_actionFlag = false;
				_reSpawnCount = 0;
			}
			else if (_mousePosX >= ACTION_X && _mousePosY >= ACTION_Y + (ACTION_HEIGHT / 3) && _mousePosX <= ACTION_X + ACTION_WIDTH && _mousePosY <= ACTION_Y + (2 * ACTION_HEIGHT / 3))
			{
				_unitTemp.push_back(SpawnUnit(UnitType::Archer, rs));
				unm->_unitList.push_back(_unitTemp.back());
				_actionFlag = false;
				_reSpawnCount = 0;
			}
			else if (_mousePosX >= ACTION_X && _mousePosY >= ACTION_Y + (2 * ACTION_HEIGHT / 3) && _mousePosX <= ACTION_X + ACTION_WIDTH && _mousePosY <= ACTION_Y + (3 * ACTION_HEIGHT / 3))
			{
				_unitTemp.push_back(SpawnUnit(UnitType::Scout, rs));
				unm->_unitList.push_back(_unitTemp.back());
				_actionFlag = false;
				_reSpawnCount = 0;
			}
		}
		else if (Mouse::IsTrigger(MOUSE_INPUT_RIGHT))
		{
			_actionFlag = false;
		}
	}

	if (tm->GetTurnCount() % 4 == 0 && !_isEneSpawn && tm->GetNowTurn() == TurnManager::TurnState::EnemyTurn)
	{
		int unitType = GetRand(2);

		if (unitType == 0)
			_unitTemp.push_back(SpawnEnemyUnit(UnitType::Soldier, rs));
		else if (unitType == 1)
			_unitTemp.push_back(SpawnEnemyUnit(UnitType::Archer, rs));
		else if (unitType == 2)
			_unitTemp.push_back(SpawnEnemyUnit(UnitType::Scout, rs));

		unm->_unitList.push_back(_unitTemp.back());
		_isEneSpawn = true;
	}
}

void BaseManager::Draw(UIManager* um)
{
	if (_actionFlag && !um->IsTargetSet())
	{
		// ベース部分の描画
		DrawBox(ACTION_X, ACTION_Y, ACTION_X + ACTION_WIDTH, ACTION_Y + ACTION_HEIGHT, color::WhiteColor, true);
		// 歩兵の生産用ボタンの描画
		DrawBox(ACTION_X, ACTION_Y, ACTION_X + ACTION_WIDTH, ACTION_Y + (ACTION_HEIGHT / 3), color::BlackColor, false);
		DrawString(ACTION_X + 30, ACTION_Y + 20, "Produce Soldier", color::BlackColor);
		// 弓兵の生産用ボタンの描画
		DrawBox(ACTION_X, ACTION_Y + (ACTION_HEIGHT / 3), ACTION_X + ACTION_WIDTH, ACTION_Y + (2 * ACTION_HEIGHT / 3), color::BlackColor, false);
		DrawString(ACTION_X + 30, ACTION_Y + (ACTION_HEIGHT / 3) + 20, "Produce Archer", color::BlackColor);
		// 工兵の生産用ボタンの描画
		DrawBox(ACTION_X, ACTION_Y + (2 * ACTION_HEIGHT / 3), ACTION_X + ACTION_WIDTH, ACTION_Y + (3 * ACTION_HEIGHT / 3), color::BlackColor, false);
		DrawString(ACTION_X + 30, ACTION_Y + (2 * ACTION_HEIGHT / 3) + 20, "Produce Scout", color::BlackColor);
	}

	/*for (auto unit : _unitTemp)
	{
		if (unit->state != UnitState::Dead)
		{
			if (!unit->isEnemy)
				DrawType(unit, color::BlackColor);
			else if (unit->isEnemy)
				DrawType(unit, color::WhiteColor);

		}
	}*/

	DrawFormatString(MAP_WIDTH, 700, 0xfffff, "%d", _reSpawnCount);
	DrawSpawnSpan();
}

_unitBase::UnitData* BaseManager::SpawnUnit(UnitType unit, RouteSearch* rs)
{
	_unitBase::UnitData* unitTemp = new _unitBase::UnitData;

	Vec2 posInd = Vec2(MY_BASE_X, MY_BASE_Y);

	unitTemp->destPos = Vec2(ENEMY_BASE_X, ENEMY_BASE_Y);
	rs->RouteSearchAstar(posInd, rs->_moveCount, unitTemp->destPos);

	unitTemp->name = "自軍追加部隊";
	unitTemp->pos = posInd;
	unitTemp->type = unit;
	ChangeStatusByType(unit, *unitTemp);
	unitTemp->color = color::YellowColor;
	unitTemp->moveTimer = 0;
	unitTemp->isEnemy = false;
	unitTemp->moveRoute = rs->GetRouteList(posInd, unitTemp->destPos);
	unitTemp->routeIndex = 0;
	unitTemp->state = UnitState::Move;

	return unitTemp;
}

_unitBase::UnitData* BaseManager::SpawnEnemyUnit(UnitType unit, RouteSearch* rs)
{
	_unitBase::UnitData* unitTemp = new _unitBase::UnitData;

	Vec2 posInd = Vec2(ENEMY_BASE_X, ENEMY_BASE_Y);

	unitTemp->destPos = Vec2(MY_BASE_X, MY_BASE_Y);
	rs->RouteSearchAstar(posInd, rs->_moveCount, unitTemp->destPos);

	unitTemp->name = "敵軍追加部隊";
	unitTemp->pos = posInd;
	unitTemp->type = unit;
	ChangeStatusByType(unit, *unitTemp);
	unitTemp->color = color::RedColor;
	unitTemp->moveTimer = 0;
	unitTemp->isEnemy = true;
	unitTemp->moveRoute = rs->GetRouteList(posInd, unitTemp->destPos);
	unitTemp->routeIndex = 0;
	unitTemp->state = UnitState::Move;

	return unitTemp;
}

void BaseManager::ChangeStatusByType(UnitType unit, _unitBase::UnitData& data)
{
	switch (unit)
	{
	case UnitType::Soldier:
		data.typeName = "Soldier";
		data.hp = 100;
		data.maxHp = data.hp;
		data.attack = 25;
		data.attackRange = 1;
		data.stamina = 10;
		data.maxStamina = data.stamina;
		return;

	case UnitType::Archer:
		data.typeName = "Archer";
		data.hp = 60;
		data.maxHp = data.hp;
		data.attack = 30;
		data.attackRange = 2;
		data.stamina = 12;
		data.maxStamina = data.stamina;
		return;

	case UnitType::Scout:
		data.typeName = "Scout";
		data.hp = 30;
		data.maxHp = data.hp;
		data.attack = 5;
		data.attackRange = 1;
		data.stamina = 20;
		data.maxStamina = data.stamina;
		return;

	default:
		printfDx("Warning : Unknown UnitType");
		break;
	}
}

//void BaseManager::DrawType(_unitBase::UnitData* data, int color)
//{
//	std::string typeInit;
//
//	switch (data->type)
//	{
//	case UnitType::Soldier:
//		typeInit = "歩";
//		break;
//	case UnitType::Archer:
//		typeInit = "弓";
//		break;
//	case UnitType::Scout:
//		typeInit = "斥";
//		break;
//	default:
//		typeInit = "?";
//		break;
//	}
//
//	DrawString(data->pos.x * NODE_SIZE, data->pos.y * NODE_SIZE, typeInit.c_str(), color);
//}

void BaseManager::DrawSpawnSpan()
{
	int cool = 4 - _reSpawnCount;

	if (cool < 0)
	{
		cool = 0;
	}

	if (cool == 0)
	{
		SetFontSize(50);
		DrawFormatString(30 * NODE_SIZE, 0, color::YellowColor, "All Ready to Produce");
		DrawFormatString(30 * NODE_SIZE + 3, 3, color::BlackColor, "All Ready to Produce");
		SetFontSize(20);
	}
	else if (cool > 0)
	{
		SetFontSize(50);
		DrawFormatString(28 * NODE_SIZE, 0, color::BlackColor, "Produce CoolingDawn...");
		DrawFormatString(50 * NODE_SIZE, 0, color::RedColor, " % d Turns Left", cool);
		DrawFormatString(50 * NODE_SIZE + 2, 2, color::BlackColor, " % d Turns Left", cool);
		SetFontSize(20);
	}
}