#include"UIManager.h"
#include"GameDefine.h"
#include"AsoDxLib/Mouse.h"
#include"AsoDxLib/Color.h"
#include"PlayerUnit.h"
#include"EnemyUnit.h"
#include"RouteSearch.h"
#include"TurnManager.h"
#include"UnitManager.h"
#include<string>

using namespace GameDefine;

UIManager::UIManager() :
	_targetSet(false),
	_targetSettingMode(false),
	_mousePosX(0),
	_mousePosY(0),
	_unitTemp(nullptr),
	_unit(nullptr),
	_buttonText(),
	_isData(false),
	_flagH(-1)
{

}

UIManager::~UIManager()
{
	DeleteGraph(_flagH);
}

void UIManager::Init()
{
	_targetSet = false;
	_targetSettingMode = false;
	_mousePosX = 0;
	_mousePosY = 0;
	_unitTemp = nullptr;
	_unit = nullptr;
	_flagH = LoadGraph("data/DestinationFlag.png");
}

void UIManager::Update(PlayerUnit* pu, EnemyUnit* eu, RouteSearch* rs, TurnManager* tm, UnitManager* unm)
{
	// マウスの座標を取得
	GetMousePoint(&_mousePosX, &_mousePosY);
	_nodeIndex = ChangePixelToIndex(Vec2((float)_mousePosX, (float)_mousePosY));

	// ターンがプレイヤー選択ターンの時のみ、マウス入力を受け付ける
	if (tm->GetNowTurn() == TurnManager::TurnState::PlayerSelectTurn)
	{
		if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
		{
			_unit = GetUnitDataFromPos(_nodeIndex, pu, eu, unm);
		}

		if (Mouse::IsTrigger(MOUSE_INPUT_RIGHT))
		{
			_isData = false;
		}

		if (_mousePosX >= SELECTING_X && _mousePosY >= SELECTING_Y && _mousePosX <= SELECTING_X + SELLECTING_WIDTH && _mousePosY <= SELECTING_Y + SELLECTING_HEIGHT)
		{
			if (Mouse::IsTrigger(MOUSE_INPUT_LEFT) && !_targetSet && !_targetSettingMode)
			{
				_targetSettingMode = true;
			}
		}

		if (_mousePosX <= (NODE_WIDTH * NODE_SIZE) && _mousePosY <= (NODE_HEIGHT * NODE_SIZE))
		{
			// 左クリックでユニットを選択、再度左クリックで目的地を設定
			if (Mouse::IsTrigger(MOUSE_INPUT_LEFT) && !_targetSet)
			{
				_unitTemp = GetUnitDataFromPos(_nodeIndex, pu, eu, unm);


				if (_unitTemp == nullptr)
				{
					_targetSettingMode = false;
					_isData = false;
				}
				else if (_unitTemp != nullptr)
				{
					if (!_unitTemp->isEnemy)
					{
						_targetSet = true;
						_isData = true;
					}
					else if (_unitTemp->isEnemy)
					{
						_targetSettingMode = false;
						_isData = true;
					}
				}
			}
			else if (Mouse::IsTrigger(MOUSE_INPUT_LEFT) && _targetSet)
			{
				if (_unitTemp != nullptr)
				{
					_unitTemp->destPos = _nodeIndex;
					rs->RouteSearchAstar(_unitTemp->pos, rs->_moveCount, _unitTemp->destPos);
					_unitTemp->moveRoute = rs->GetRouteList(_unitTemp->pos, _unitTemp->destPos);
					_unitTemp->routeIndex = 0;
					_unitTemp->state = UnitState::Move;
					_unitTemp = nullptr;
					_targetSet = false;
					_targetSettingMode = false;
					_isData = false;
				}
				// 右クリックで選択を解除
				else
				{
					_unitTemp = nullptr;
					_targetSet = false;
					_targetSettingMode = false;
					_isData = false;
				}
			}
			else if (Mouse::IsTrigger(MOUSE_INPUT_RIGHT) && _targetSet)
			{
				_unitTemp = nullptr;
				_targetSet = false;
				_targetSettingMode = false;
				_isData = false;
			}

			if (_unitTemp == nullptr)
			{
				_targetSet = false;
			}

		}
	}
}

void UIManager::Draw(RouteSearch* rs)
{
	if (_targetSet && _unitTemp != nullptr)
	{
		rs->RouteSearchAstar(_unitTemp->pos, rs->_moveCount, Vec2(_mousePosX / NODE_SIZE, _mousePosY / NODE_SIZE));
		rs->DrawRoute(Vec2(_mousePosX / NODE_SIZE, _mousePosY / NODE_SIZE));

		DrawGraph((_mousePosX / NODE_SIZE) * NODE_SIZE, (_mousePosY / NODE_SIZE) * NODE_SIZE, _flagH, true);
	}

	if (!_targetSettingMode)
	{
		_buttonText = "Change Target";
	}
	else if (_targetSettingMode)
	{
		_buttonText = "Selecting...";
	}

	DrawBox(SELECTING_X, SELECTING_Y, SELECTING_X + SELLECTING_WIDTH, SELECTING_Y + SELLECTING_HEIGHT, color::CyanColor, true);
	DrawFormatString(SELECTING_X + 40, SELECTING_Y, color::BlackColor, _buttonText.c_str());

	// マウスの選択中グリッドを強調
	if (_mousePosX <= MAP_WIDTH && _mousePosY <= MAP_HEIGHT)
		DrawBox((_mousePosX / NODE_SIZE) * NODE_SIZE, (_mousePosY / NODE_SIZE) * NODE_SIZE, ((_mousePosX + NODE_SIZE) / NODE_SIZE) * NODE_SIZE, ((_mousePosY + NODE_SIZE) / NODE_SIZE) * NODE_SIZE, color::RedColor, false);

	if (_isData)
		DrawUnitData(_unit);

}

Vec2 UIManager::ChangePixelToIndex(Vec2 mousePos)
{
	int mouseIndX = (int)(mousePos.x / NODE_SIZE);
	int mouseIndY = (int)(mousePos.y / NODE_SIZE);

	Vec2 mouseIndex = Vec2((float)mouseIndX, (float)mouseIndY);

	return mouseIndex;
}

_unitBase::UnitData* UIManager::GetUnitDataFromPos(Vec2 mousePos, PlayerUnit* pu, EnemyUnit* eu, UnitManager* unm) const
{
	for (auto& unit : unm->_unitList)
	{
		if (unit->state == UnitState::Dead) continue;
		if (mousePos == unit->pos) return unit;

	}
	return nullptr;
}

void UIManager::DrawUnitData(_unitBase::UnitData* data)
{
	std::string tempName;
	Vec2 tempPos;
	std::string tempType;
	int tempHp;
	int tempMaxHp;
	int tempAttack;

	if (data == nullptr)
	{
		return;
	}
	else
	{
		tempName = data->name;
		tempPos = data->destPos;
		tempType = data->typeName;
		tempHp = data->hp;
		tempMaxHp = data->maxHp;
		tempAttack = data->attack;
	}

	DrawData(Vec2((data->pos.x + 1) * NODE_SIZE, (data->pos.y * NODE_SIZE)), tempName, tempPos, tempType, tempHp, tempMaxHp, tempAttack);
}

void UIManager::DrawData(Vec2 pos, std::string name, Vec2 unitPos, std::string type, int hp, int maxHp, int attack)
{
	DrawBox(pos.x, pos.y, pos.x + DATA_WIDTH, pos.y + DATA_HEIGHT, color::WhiteColor, true);
	DrawFormatString(pos.x + 10, pos.y, color::BlackColor, name.c_str());
	DrawFormatString(pos.x + 10, pos.y + 20, color::BlackColor, "Destination : %.0f, %.0f", unitPos.x, unitPos.y);
	DrawFormatString(pos.x + 10, pos.y + 40, color::BlackColor, type.c_str());
	DrawFormatString(pos.x + 10, pos.y + 60, color::BlackColor, "Hp : %d / %d", hp, maxHp);
	DrawFormatString(pos.x + 10, pos.y + 80, color::BlackColor, "AttackDamage : %d", attack);
}