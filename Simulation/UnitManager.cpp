#include"UnitManager.h"
#include"AsoDxLib/Color.h"
#include"AsoDxLib/Keyboard.h"

UnitManager::UnitManager() :
	p_PlayerUnit(nullptr),
	p_EnemyUnit(nullptr),
	_finishCount(0),
	_occupiedMap(true)
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
}

void UnitManager::Update(RouteSearch* rs, TurnManager* tm)
{
	Keyboard::Update();

	p_PlayerUnit->Update();
	p_EnemyUnit->Update();

	switch (tm->GetNowTurn())
	{
	case TurnManager::TurnState::PlayerSelectTurn:
		if (Keyboard::IsTrigger(KEY_INPUT_SPACE)) tm->TurnChange();


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

		/*memset(_occupiedMap, false, sizeof(_occupiedMap));
		for (auto& unit : _unitList)
			if (unit->state != UnitState::Dead)
				_occupiedMap[(int)unit->pos.y][(int)unit->pos.x] = true;*/

		break;

	case TurnManager::TurnState::SelectResultTurn:
		_finishCount = 0;

		// 行動中のユニットの状態を更新
		for (auto& unit : _unitList)
		{
			if (!unit->isEnemy)
			{
				if (unit->state == UnitState::Dead)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm);
				}
				else if (unit->moveTimer > GameDefine::MOVE_SPAN)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm);
				}

				if (unit->state == UnitState::Idle || unit->state == UnitState::Arrived || unit->state == UnitState::Dead)
				{
					_finishCount++;
				}
			}
		}

		if (_finishCount > 1) tm->TurnChange();
		break;

	case TurnManager::TurnState::EnemyTurn:
		_finishCount = 0;

		// 行動中のユニットの状態を更新
		for (auto& unit : _unitList)
		{
			if (unit->isEnemy)
			{
				if (unit->state == UnitState::Dead)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm);
				}
				else if (unit->moveTimer > GameDefine::MOVE_SPAN)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm);
				}

				if (unit->state == UnitState::Idle || unit->state == UnitState::Arrived || unit->state == UnitState::Dead)
				{
					_finishCount++;
				}
			}
		}

		if (_finishCount > 1)
		{
			tm->TurnChange();
			tm->_turnCount++;
		}
		break;

	default:
		break;
	}
}

void UnitManager::Draw()
{
	int row = 0;

	for (auto& unit : _unitList)
	{
		if (unit->state != UnitState::Dead)
			DrawBox((unit->pos.x) * GameDefine::NODE_SIZE, (unit->pos.y) * GameDefine::NODE_SIZE, (unit->pos.x + 1) * GameDefine::NODE_SIZE, (unit->pos.y + 1) * GameDefine::NODE_SIZE, unit->color, true);
		DrawFormatString(0, row, 0xffffff, "%d", unit->stamina);
		row += 20;
	}
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

void UnitManager::StateAttack(_unitBase::UnitData& data, int& timer)
{
	timer = 0;
	// 攻撃済みの場合は待機状態に遷移
	if (data.hasAttacked == true)
	{
		data.state = UnitState::Idle;
		return;
	}

	printfDx("交戦中！\n");
	// 攻撃範囲内に敵がいるか確認
	for (auto& unit : _unitList)
	{
		if (&data == unit) continue;
		if (data.isEnemy == unit->isEnemy) continue;
		if (unit->state == UnitState::Dead) continue;

		int distance = Distance(&data, unit);

		if (distance <= data.attackRange && data.state != UnitState::Dead && unit->state != UnitState::Dead)
		{
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
		StateAttack(data, timer);
		break;

	case UnitState::Dead:
		StateDead(data, timer);
		break;

	default:
		break;
	}
}