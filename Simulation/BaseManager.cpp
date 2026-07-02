#include "BaseManager.h"
#include"GameDefine.h"
#include"AsoDxLib/Mouse.h"

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

void BaseManager::Update(RouteSearch* rs, UIManager* um, UnitManager* unm)
{
	GetMousePoint(&_mousePosX, &_mousePosY);

	for (auto& unit : _unitTemp)
	{
		unit->moveTimer++;
	}

	if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
	{
		if (_mousePosX <= NODE_WIDTH * NODE_SIZE && _mousePosY <= NODE_HEIGHT * NODE_SIZE)
		{
			Vec2 _mouseNodePos = um->ChangePixelToIndex(Vec2(_mousePosX, _mousePosY));
			_tileTemp = rs->GetNodeData(_mouseNodePos.x, _mouseNodePos.y);

			if (_tileTemp == TileType::MyBase)
			{
				_actionFlag = true;
			}
		}
	}

	if (_actionFlag)
	{
		if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
		{
			if (_mousePosX >= ACTION_X && _mousePosY >= ACTION_Y && _mousePosX <= ACTION_X + ACTION_WIDTH && _mousePosY <= ACTION_Y + (ACTION_HEIGHT / 3))
			{
				_unitTemp.push_back(SpawnUnit(UnitType::Soldier, rs));
				unm->_unitList.push_back(_unitTemp.back());
				_actionFlag = false;
			}
			else if (_mousePosX >= ACTION_X && _mousePosY >= ACTION_Y + (ACTION_HEIGHT / 3) && _mousePosX <= ACTION_X + ACTION_WIDTH && _mousePosY <= ACTION_Y + (2 * ACTION_HEIGHT / 3))
			{
				_unitTemp.push_back(SpawnUnit(UnitType::Archer, rs));
				unm->_unitList.push_back(_unitTemp.back());
				_actionFlag = false;
			}
			else if (_mousePosX >= ACTION_X && _mousePosY >= ACTION_Y + (2 * ACTION_HEIGHT / 3) && _mousePosX <= ACTION_X + ACTION_WIDTH && _mousePosY <= ACTION_Y + (3 * ACTION_HEIGHT / 3))
			{
				_unitTemp.push_back(SpawnUnit(UnitType::Engineer, rs));
				unm->_unitList.push_back(_unitTemp.back());
				_actionFlag = false;
			}
		}
		else if (Mouse::IsTrigger(MOUSE_INPUT_RIGHT))
		{
			_actionFlag = false;
		}
	}
}

void BaseManager::Draw()
{
	if (_actionFlag)
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
		DrawString(ACTION_X + 30, ACTION_Y + (2 * ACTION_HEIGHT / 3) + 20, "Produce Engineer", color::BlackColor);
	}
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

void BaseManager::ChangeStatusByType(UnitType unit, _unitBase::UnitData& data)
{
	switch (unit)
	{
	case UnitType::Soldier:
		data.typeName = "Soldier";
		data.hp = 100;
		data.attack = 25;
		data.attackRange = 1;
		data.stamina = 10;
		data.maxStamina = data.stamina;
		data.canBuilding = false;
		return;

	case UnitType::Archer:
		data.typeName = "archer";
		data.hp = 60;
		data.attack = 30;
		data.attackRange = 2;
		data.stamina = 12;
		data.maxStamina = data.stamina;
		data.canBuilding = false;
		return;

	case UnitType::Engineer:
		data.typeName = "Engineer";
		data.hp = 30;
		data.attack = 5;
		data.attackRange = 1;
		data.stamina = 7;
		data.maxStamina = data.stamina;
		data.canBuilding = true;
		return;

	default:
		printfDx("Warning: Unknown UnitType");
		break;
	}
}