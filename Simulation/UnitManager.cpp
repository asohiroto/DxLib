#include"UnitManager.h"
#include"AsoDxLib/Mouse.h"
#include"AsoDxLib/Color.h"
#include"PlayerUnit.h"
#include"EnemyUnit.h"
#include"RouteSearch.h"
#include"TurnManager.h"
#include"SceneManager.h"
#include"ClearScene.h"
#include"GameOverScene.h"
#include"SceneBase.h"
#include<string>

using namespace GameDefine;

UnitManager::UnitManager() :
	p_SceneBase(nullptr),
	p_PlayerUnit(nullptr),
	p_EnemyUnit(nullptr),
	_finishCount(0),
	_occupiedMap(true),
	_playerCount(0),
	_enemyCount(0),
	_unitsH(-1)
{

}

UnitManager::~UnitManager()
{
	delete p_PlayerUnit;
	delete p_EnemyUnit;
	delete p_SceneBase;
	DeleteGraph(_unitsH);
}

void UnitManager::Init(RouteSearch* rs, SceneManager& _sceneManager)
{
	_unitsH = LoadGraph("data/ユニット.png");

	p_PlayerUnit = new PlayerUnit;
	p_EnemyUnit = new EnemyUnit;
	p_SceneBase = new SceneBase(_sceneManager);

	p_PlayerUnit->Init(rs);
	p_EnemyUnit->Init(rs);

	// _unitListにユニットのポインタを追加
	_unitList.push_back(&p_PlayerUnit->GetMainUnit());
	_unitList.push_back(&p_PlayerUnit->GetSubUnit());
	_unitList.push_back(&p_EnemyUnit->GetMainUnit());
	_unitList.push_back(&p_EnemyUnit->GetSubUnit());

	memset(_occupiedMap, false, sizeof(_occupiedMap));

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

void UnitManager::Update(RouteSearch* rs, TurnManager* tm, SceneManager& sm)
{
	p_PlayerUnit->Update();
	p_EnemyUnit->Update();

	if (CheckHitKey(KEY_INPUT_SPACE))
	{
		sm.ChangeScene(std::make_shared<ClearScene>(sm));
	}

	if (CheckHitKey(KEY_INPUT_RETURN))
	{
		sm.ChangeScene(std::make_shared<GameOverScene>(sm));
	}

	for (auto& unit : _unitList)
	{
		if (unit->isHeapAllocated)
		{
			unit->moveTimer++;
		}
	}

	switch (tm->GetNowTurn())
	{
	case TurnManager::TurnState::PlayerSelectTurn:
		GetMousePoint(&_mousePosX, &_mousePosY);

		if (Mouse::IsTrigger(MOUSE_INPUT_LEFT))
		{
			if (TURNBUTTON_X <= _mousePosX && _mousePosX <= TURNBUTTON_X + TURNBUTTON_WIDTH && TURNBUTTON_Y <= _mousePosY && _mousePosY <= TURNBUTTON_Y + TURNBUTTON_HEIGHT)
			{
				tm->TurnChange();
			}
		}

		for (auto& unit : _unitList)
		{
			if (unit->hp <= 0) unit->state = UnitState::Dead;

			// 行動開始前にスタミナを回復する
			unit->stamina = unit->maxStamina;
			unit->hasAttacked = false;

			if (unit->isEnemy && !unit->hasAttacked && unit->state != UnitState::Dead)
			{
				if (rs->GetNodeData(unit->pos.x, unit->pos.y) == TileType::EnemyBase)
				{
					unit->state = UnitState::Attack;
				}
			}

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
					DrawBox(unit->pos.x * NODE_SIZE, unit->pos.y * NODE_SIZE, (unit->pos.x + 1) * NODE_SIZE, (unit->pos.y + 1) * NODE_SIZE, color::RedColor, false, 3);

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
					SetMoveByState(*unit, unit->moveTimer, rs, tm, sm);
				}
				else if (unit->moveTimer > MOVE_SPAN)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm, sm);
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
					SetMoveByState(*unit, unit->moveTimer, rs, tm, sm);
				}
				else if (unit->moveTimer > MOVE_SPAN)
				{
					SetMoveByState(*unit, unit->moveTimer, rs, tm, sm);
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

	RemoveDeadUnits();
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
		{
			int num = GetUnitNum(unit);

			int x = 25 * num;
			int y = 0;

			DrawRectGraph((unit->pos.x) * NODE_SIZE, (unit->pos.y) * NODE_SIZE, x, y, 25, 25, _unitsH, true);

			DrawBox((unit->pos.x) * NODE_SIZE - 10, (unit->pos.y) * NODE_SIZE - 10, (unit->pos.x + 1) * NODE_SIZE + 10, (unit->pos.y) * NODE_SIZE - 2, color::RedColor, true);
			DrawBox((unit->pos.x) * NODE_SIZE - 10, (unit->pos.y) * NODE_SIZE - 10, (unit->pos.x) * NODE_SIZE + (35 * unit->hp / unit->maxHp), (unit->pos.y) * NODE_SIZE - 2, color::GreenColor, true);
			DrawBox((unit->pos.x) * NODE_SIZE - 10, (unit->pos.y) * NODE_SIZE - 10, (unit->pos.x + 1) * NODE_SIZE + 10, (unit->pos.y) * NODE_SIZE - 2, color::BlackColor, false);
		}
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

	if (!data.hasAttacked && data.state != UnitState::Dead)
	{
		if (!data.isEnemy && rs->GetNodeData(data.pos.x, data.pos.y) == TileType::EnemyBase)
		{
			data.state = UnitState::Attack;
			return;
		}
		else if (data.isEnemy && rs->GetNodeData(data.pos.x, data.pos.y) == TileType::MyBase)
		{
			data.state = UnitState::Attack;
			return;
		}
	}

	// 経路が空、またはルートのインデックスが範囲外の場合、到着状態に遷移
	if (data.moveRoute.empty() || data.routeIndex >= (int)data.moveRoute.size())
	{
		//printfDx("到着\n");
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
			_occupiedMap[(int)data.pos.y][(int)data.pos.x] = true;
			data.routeIndex++;
			return; // 次のフレームで実際の次のマスへ移動開始
		}

		// 次のマスが予約済みなら待機
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

void UnitManager::StateAttack(_unitBase::UnitData& data, int& timer, RouteSearch* rs, SceneManager& sm)
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
			//printfDx("交戦中！\n");

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
				//printfDx("攻撃中\n");

				data.hp -= _enemyBaseAttack;
				_enemyBaseHpNow -= data.attack;

				data.hasAttacked = true;

				if (_enemyBaseHpNow <= 0)
				{
					//printfDx("破壊\n");
					sm.ChangeScene(std::make_shared<ClearScene>(sm));
					break;
				}
				data.state = UnitState::Idle;
			}
		}
		else if (data.isEnemy && !data.hasAttacked && _myBaseHpNow > 0)
		{
			if (rs->GetNodeData(_nextPos.x, _nextPos.y) == TileType::MyBase || rs->GetNodeData(data.pos.x, data.pos.y) == TileType::MyBase)
			{
				//printfDx("攻撃中\n");

				data.hp -= _myBaseAttack;
				_myBaseHpNow -= data.attack;

				data.hasAttacked = true;

				if (_myBaseHpNow <= 0)
				{
					//printfDx("破壊\n");
					sm.ChangeScene(std::make_shared<GameOverScene>(sm));
					break;
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

void UnitManager::SetMoveByState(_unitBase::UnitData& data, int& timer, RouteSearch* rs, TurnManager* tm, SceneManager& sm)
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
		StateAttack(data, timer, rs, sm);
		break;

	case UnitState::Dead:
		StateDead(data, timer);
		break;

	default:
		break;
	}
}

int UnitManager::GetUnitNum(_unitBase::UnitData* unit)
{
	switch (unit->type)
	{
	case UnitType::Soldier:
		if (unit->isEnemy) return 0;
		else if (!unit->isEnemy) return 3;

	case UnitType::Archer:
		if (unit->isEnemy) return 1;
		else if (!unit->isEnemy) return 4;

	case UnitType::Scout:
		if (unit->isEnemy) return 2;
		else if (!unit->isEnemy) return 5;

	default:
		return -1;
	}
}

// ユニットリストから死亡したユニットを削除する関数
void UnitManager::RemoveDeadUnits()
{
	// ユニットリストをループ
	for (auto it = _unitList.begin(); it != _unitList.end();)
	{
		// ユニットが死亡していて、ヒープ領域に確保されている場合は削除
		if ((*it)->state == UnitState::Dead && (*it)->isHeapAllocated)
		{
			delete* it;
			it = _unitList.erase(it);
		}
		// ユニットが死亡していない場合は次のユニットへ
		else ++it;
	}
}