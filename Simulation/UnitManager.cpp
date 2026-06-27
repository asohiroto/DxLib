#include"UnitManager.h"
#include"PlayerUnit.h"
#include"EnemyUnit.h"
#include"AsoDxLib/Color.h"

UnitManager::UnitManager() :
	p_PlayerUnit(nullptr),
	p_EnemyUnit(nullptr),
	timeStop(false)
{

}

UnitManager::~UnitManager()
{

}

void UnitManager::Init(RouteSearch* rs)
{
	p_PlayerUnit = new PlayerUnit;
	p_EnemyUnit = new EnemyUnit;

	p_PlayerUnit->Init(rs);
	p_EnemyUnit->Init(rs);

	_unitList.push_back(&p_PlayerUnit->GetMainUnit());
	_unitList.push_back(&p_PlayerUnit->GetSubUnit());
	_unitList.push_back(&p_EnemyUnit->GetMainUnit());
	_unitList.push_back(&p_EnemyUnit->GetSubUnit());
}

void UnitManager::Update()
{
	if (CheckHitKey(KEY_INPUT_Z)) timeStop = true;
	if (CheckHitKey(KEY_INPUT_X)) timeStop = false;

	if (!timeStop)
	{
		p_PlayerUnit->Update();
		p_EnemyUnit->Update();

		for (auto& unit : _unitList)
		{
			if (unit->moveTimer > 10)
				SetMoveByState(*unit, unit->moveTimer);
		}
	}
}

void UnitManager::Draw()
{
	p_PlayerUnit->Draw();
	p_EnemyUnit->Draw();
}

void UnitManager::StateMove(_unitBase::UnitData& data, int& timer)
{
	timer = 0;
	if (data.moveRoute.empty() || data.routeIndex >= (int)data.moveRoute.size())
	{
		data.state = UnitState::Arrived;
		return;
	}

	data.pos = data.moveRoute[data.routeIndex];

	data.state = UnitState::Idle;
}

void UnitManager::StateIdle(_unitBase::UnitData& data, int& timer)
{
	timer = 0;
	if (data.pos.x == GameDefine::ENEMY_BASE_X && data.pos.y == GameDefine::ENEMY_BASE_Y)
	{
		data.state = UnitState::Arrived;
		return;
	}

	data.routeIndex++;

	for (auto& unit : _unitList)
	{
		if (&data == unit) continue;

		int distance = Distance(&data, unit);
		if (distance <= data.attackRange && data.isEnemy != unit->isEnemy)
		{
			data.state = UnitState::Attack;
			return;
		}
	}

	data.state = UnitState::Move;
}

void UnitManager::StateArrived(_unitBase::UnitData& data, int& timer)
{
	timer = 0;
}

void UnitManager::StateAttack(_unitBase::UnitData& data, int& timer)
{
	timer = 0;
	printfDx("交戦中！\n");
	for (auto& unit : _unitList)
	{
		if (&data == unit) continue;
		if (data.isEnemy == unit->isEnemy) continue;
		if (unit->state == UnitState::Dead) continue;

		int distance = Distance(&data, unit);

		if (distance <= data.attackRange && data.state != UnitState::Dead && unit->state != UnitState::Dead)
		{
			unit->hp -= data.attack;

			if (data.hp < 0) data.state = UnitState::Dead;
			return;
		}
	}
	data.state = UnitState::Move;
}

void UnitManager::StateDead(_unitBase::UnitData& data, int& timer)
{
	timer = 0;
	data.color = color::BlackColor;
}

int UnitManager::Distance(_unitBase::UnitData* player, _unitBase::UnitData* enemy)
{
	int disX = abs(enemy->pos.x - player->pos.x);
	int disY = abs(enemy->pos.y - player->pos.y);

	return disX + disY;
}

void UnitManager::SetMoveByState(_unitBase::UnitData& data, int& timer)
{
	switch (data.state)
	{
	case UnitState::Move:
		StateMove(data, timer);
		break;

	case UnitState::Arrived:
		StateArrived(data, timer);
		break;

	case UnitState::Idle:
		StateIdle(data, timer);
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