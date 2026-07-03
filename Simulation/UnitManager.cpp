#include"UnitManager.h"
#include"AsoDxLib/Mouse.h"
#include"AsoDxLib/Keyboard.h"
#include"AsoDxLib/Color.h"
#include<string>

using namespace GameDefine;

UnitManager::UnitManager() :
	p_PlayerUnit(nullptr),
	p_EnemyUnit(nullptr),
	_finishCount(0),
	_occupiedMap(true),
	_playerCount(0),
	_enemyCount(0)
{

}

UnitManager::~UnitManager()
{
	delete p_PlayerUnit;
	delete p_EnemyUnit;
}

void UnitManager::Init(RouteSearch* rs)
{
	p_PlayerUnit = new PlayerUnit;
	p_EnemyUnit = new EnemyUnit;

	p_PlayerUnit->Init(rs);
	p_EnemyUnit->Init(rs);

	// _unitListにユニットのポインタを追加
	_unitList.push_back(&p_PlayerUnit->GetMainUnit());
	_unitList.push_back(&p_PlayerUnit->GetSubUnit());
	_unitList.push_back(&p_EnemyUnit->GetMainUnit());
	_unitList.push_back(&p_EnemyUnit->GetSubUnit());

	_finishCount = 0;
	_playerCount = 0;
	_enemyCount = 0;

	_myBaseHpMax = 150;
	_enemyBaseHpMax = 150;
	_myBaseHpNow = _myBaseHpMax;
	_enemyBaseHpNow = _enemyBaseHpMax;
	_myBaseAttack = 20;
	_enemyBaseAttack = 20;
}

void UnitManager::Update(RouteSearch* rs, TurnManager* tm)
{
	p_PlayerUnit->Update();
	p_EnemyUnit->Update();

	switch (tm->GetNowTurn())
	{
	case TurnManager::TurnState::PlayerSelectTurn:
		GetMousePoint(&_mousePosX, &_mousePosY);

		if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
		{
			if ((NODE_WIDTH * NODE_SIZE) <= _mousePosX && _mousePosX <= (NODE_WIDTH * NODE_SIZE + 200) && (NODE_HEIGHT * NODE_SIZE) <= _mousePosY && _mousePosY <= (NODE_HEIGHT * NODE_SIZE + 40))
			{
				tm->TurnChange();
			}
		}

		for (auto& unit : _unitList)
		{
			// 行動開始前にスタミナを回復する
			unit->stamina = unit->maxStamina;
			unit->hasAttacked = false;

			if (unit->state == UnitState::Arrived) unit->state = UnitState::Idle;

			if (unit->state == UnitState::Idle)
			{
				// 攻撃する相手がいるか
				bool canAttack = false;

				// 攻撃範囲内に敵がいるかを確認
				for (auto& data : _unitList)
				{
					if (unit == data) continue;
					if (unit->isEnemy == data->isEnemy) continue;
					if (data->state == UnitState::Dead) continue;

					int distance = Distance(unit, data);

					if (distance <= unit->attackRange && unit->state != UnitState::Dead && data->state != UnitState::Dead)
					{
						canAttack = true;
						break;
					}
				}

				for (int i = 0; i < unit->attackRange; i++)
				{
					if (unit->routeIndex + i >= (int)unit->moveRoute.size()) break;

					_nextPos = unit->moveRoute[unit->routeIndex + i];

					if (!unit->isEnemy && !unit->hasAttacked && unit->state != UnitState::Dead)
					{
						if (rs->GetNodeData(_nextPos.x, _nextPos.y) == TileType::EnemyBase || rs->GetNodeData(unit->pos.x, unit->pos.y) == TileType::EnemyBase)
						{
							unit->state = UnitState::Attack;
							canAttack = true;
						}
					}
					else if (unit->isEnemy && !unit->hasAttacked && unit->state != UnitState::Dead)
					{
						if (rs->GetNodeData(_nextPos.x, _nextPos.y) == TileType::MyBase || rs->GetNodeData(unit->pos.x, unit->pos.y) == TileType::MyBase)
						{
							unit->state = UnitState::Attack;
							canAttack = true;
						}
					}
				}

				if (canAttack)
				{
					unit->state = UnitState::Attack;
				}
				else
				{
					unit->state = UnitState::Move;
				}
			}
		}

		break;

	case TurnManager::TurnState::SelectResultTurn:
		_finishCount = 0;

		// 行動中のユニットの状態を更新
		for (auto& unit : _unitList)
		{
			if (!unit->isEnemy)
			{
				_playerCount++;
				if (unit->state == UnitState::Dead)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm);
				}
				else if (unit->moveTimer > MOVE_SPAN)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm);
				}

				if (unit->state == UnitState::Idle || unit->state == UnitState::Arrived || unit->state == UnitState::Dead)
				{
					_finishCount++;
				}
			}
		}

		if (_finishCount >= _playerCount) tm->TurnChange();

		_playerCount = 0;
		break;

	case TurnManager::TurnState::EnemyTurn:
		_finishCount = 0;

		// 行動中のユニットの状態を更新
		for (auto& unit : _unitList)
		{
			if (unit->isEnemy)
			{
				_enemyCount++;
				if (unit->state == UnitState::Dead)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm);
				}
				else if (unit->moveTimer > MOVE_SPAN)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm);
				}

				if (unit->state == UnitState::Idle || unit->state == UnitState::Arrived || unit->state == UnitState::Dead)
				{
					_finishCount++;
				}
			}
		}

		if (_finishCount >= _enemyCount)
		{
			tm->TurnChange();
			tm->_turnCount++;
		}

		_enemyCount = 0;
		break;

	default:
		break;
	}
}

void UnitManager::Draw(TurnManager* tm)
{
	int row = 0;
	std::string turnButton;
	int addSpaceX;
	int addSpaceY;

	for (auto& unit : _unitList)
	{
		if (unit->state != UnitState::Dead)
			DrawBox((unit->pos.x) * NODE_SIZE, (unit->pos.y) * NODE_SIZE, (unit->pos.x + 1) * NODE_SIZE, (unit->pos.y + 1) * NODE_SIZE, unit->color, true);
		DrawFormatString(0, row, 0xffffff, "%d", unit->stamina);
		row += 20;
	}

	p_PlayerUnit->Draw();
	p_EnemyUnit->Draw();

	if (tm->GetNowTurn() == TurnManager::TurnState::PlayerSelectTurn)
	{
		turnButton = "Next Turn";
		addSpaceX = 55;
		addSpaceY = 10;
	}
	else
	{
		turnButton = "Turn Transition...";
		addSpaceX = 20;
		addSpaceY = 10;
	}

	DrawBox(TURNBUTTON_X, TURNBUTTON_Y, TURNBUTTON_X + TURNBUTTON_WIDTH, TURNBUTTON_Y + TURNBUTTON_HEIGHT, color::OrangeColor, true);

	DrawFormatString(TURNBUTTON_X + addSpaceX, TURNBUTTON_Y + addSpaceY, 0x000000, turnButton.c_str());
}

void UnitManager::StateMove(_unitBase::UnitData& data, int& timer, RouteSearch* rs)
{
	timer = 0;

	// 経路が空、またはルートのインデックスが範囲外の場合、到着状態に遷移
	if (data.moveRoute.empty() || data.routeIndex >= (int)data.moveRoute.size())
	{
		printfDx("到着\n");
		data.state = UnitState::Arrived;
		return;
	}

	int moveCost = rs->GetMoveCost(rs->_fieldTbl[(int)data.moveRoute[data.routeIndex].y][(int)data.moveRoute[data.routeIndex].x]);

	// スタミナが足りる場合のみ移動
	if ((data.stamina - moveCost) >= 0)
	{
		Vec2 nextPos = data.moveRoute[data.routeIndex];

		if (nextPos.x == data.pos.x && nextPos.y == data.pos.y)
		{
			data.routeIndex++;
			return; // 次のフレームで実際の次のマスへ移動開始
		}

		if (_occupiedMap[(int)nextPos.y][(int)nextPos.x])
		{
			data.state = UnitState::Idle;
			return;
		}

		// 移動先のマスを予約済みにする
		_occupiedMap[(int)nextPos.y][(int)nextPos.x] = true;
		_occupiedMap[(int)data.pos.y][(int)data.pos.x] = false;

		data.pos = nextPos;
		data.routeIndex++;
		data.stamina -= moveCost;
	}
	else
	{
		data.state = UnitState::Idle;
	}

	// 移動後に攻撃範囲内に敵がいるか確認
	for (auto& unit : _unitList)
	{
		if (&data == unit) continue;
		if (unit->state == UnitState::Dead) continue;

		int distance = Distance(&data, unit);

		if (distance <= data.attackRange && data.isEnemy != unit->isEnemy && data.state != UnitState::Dead)
		{
			data.state = UnitState::Attack;
			return;
		}
	}


	for (int i = 0; i < data.attackRange; i++)
	{
		if (data.routeIndex + i >= (int)data.moveRoute.size()) break;

		_nextPos = data.moveRoute[data.routeIndex + i];

		if (!data.isEnemy && !data.hasAttacked && data.state != UnitState::Dead)
		{
			if (rs->GetNodeData(_nextPos.x, _nextPos.y) == TileType::EnemyBase || rs->GetNodeData(data.pos.x, data.pos.y) == TileType::EnemyBase)
			{
				data.state = UnitState::Attack;
			}
		}
		else if (data.isEnemy && !data.hasAttacked && data.state != UnitState::Dead)
		{
			if (rs->GetNodeData(_nextPos.x, _nextPos.y) == TileType::MyBase || rs->GetNodeData(data.pos.x, data.pos.y) == TileType::MyBase)
			{
				data.state = UnitState::Attack;
			}
		}

		break;
	}
}

void UnitManager::StateIdle(_unitBase::UnitData& data, int& timer, TurnManager* tm)
{
	timer = 0;
	if (data.pos.x == GameDefine::ENEMY_BASE_X && data.pos.y == GameDefine::ENEMY_BASE_Y)
	{
		data.state = UnitState::Arrived;
		return;
	}
}

void UnitManager::StateArrived(_unitBase::UnitData& data, int& timer)
{
	timer = 0;
}

void UnitManager::StateAttack(_unitBase::UnitData& data, int& timer, RouteSearch* rs)
{
	timer = 0;
	// 攻撃済みの場合は待機状態に遷移
	if (data.hasAttacked == true)
	{
		data.state = UnitState::Idle;
		return;
	}

	// 攻撃範囲内に敵がいるか確認
	for (auto& unit : _unitList)
	{
		if (&data == unit) continue;
		if (data.isEnemy == unit->isEnemy) continue;
		if (unit->state == UnitState::Dead) continue;

		int distance = Distance(&data, unit);

		if (distance <= data.attackRange && data.state != UnitState::Dead && unit->state != UnitState::Dead)
		{
			printfDx("交戦中！\n");

			// 攻撃処理
			unit->hp -= data.attack;
			data.hasAttacked = true;

			if (unit->hp <= 0)
			{
				unit->state = UnitState::Dead;
			}
			data.state = UnitState::Idle;
			return;
		}
	}


	for (int i = 0; i < data.attackRange; i++)
	{
		if (data.routeIndex + i >= (int)data.moveRoute.size()) break;

		_nextPos = data.moveRoute[data.routeIndex + i];

		if (!data.isEnemy && !data.hasAttacked && _enemyBaseHpNow > 0)
		{
			if (rs->GetNodeData(_nextPos.x, _nextPos.y) == TileType::EnemyBase || rs->GetNodeData(data.pos.x, data.pos.y) == TileType::EnemyBase)
			{
				printfDx("攻撃中\n");

				data.hp -= _enemyBaseAttack;
				_enemyBaseHpNow -= data.attack;

				data.hasAttacked = true;

				if (_enemyBaseHpNow <= 0)
				{
					printfDx("破壊\n");
				}
				data.state = UnitState::Idle;
			}
		}
		else if (data.isEnemy && !data.hasAttacked && _myBaseHpNow > 0)
		{
			if (rs->GetNodeData(_nextPos.x, _nextPos.y) == TileType::MyBase || rs->GetNodeData(data.pos.x, data.pos.y) == TileType::MyBase)
			{
				printfDx("攻撃中\n");

				data.hp -= _myBaseAttack;
				_myBaseHpNow -= data.attack;

				data.hasAttacked = true;

				if (_myBaseHpNow <= 0)
				{
					printfDx("破壊\n");
				}
				data.state = UnitState::Idle;
			}
		}
	}

	data.state = UnitState::Idle;
}

void UnitManager::StateDead(_unitBase::UnitData& data, int& timer)
{
	timer = 0;
	_occupiedMap[(int)data.pos.y][(int)data.pos.x] = false;
}

int UnitManager::Distance(_unitBase::UnitData* player, _unitBase::UnitData* enemy)
{
	int disX = abs(enemy->pos.x - player->pos.x);
	int disY = abs(enemy->pos.y - player->pos.y);

	return disX + disY;
}

void UnitManager::SetMoveByState(_unitBase::UnitData& data, int& timer, RouteSearch* rs, TurnManager* tm)
{
	switch (data.state)
	{
	case UnitState::Move:
		StateMove(data, timer, rs);
		break;

	case UnitState::Arrived:
		StateArrived(data, timer);
		break;

	case UnitState::Idle:
		StateIdle(data, timer, tm);
		break;

	case UnitState::Attack:
		StateAttack(data, timer, rs);
		break;

	case UnitState::Dead:
		StateDead(data, timer);
		break;

	default:
		break;
	}
}