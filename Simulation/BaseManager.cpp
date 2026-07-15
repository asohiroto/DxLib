#include "BaseManager.h"
#include"GameDefine.h"
#include"RouteSearch.h"
#include"UIManager.h"
#include"UnitManager.h"
#include"AsoDxLib/Mouse.h"
#include"TurnManager.h"
#include"_unitBase.h"

using namespace GameDefine;

BaseManager::BaseManager() :
	_mousePosX(0),
	_mousePosY(0),
	_tileTemp(),
	_actionFlag(false)
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
	// マウス位置の取得
	GetMousePoint(&_mousePosX, &_mousePosY);

	// プレイヤー選択ターンになると、エネミーをスポーンできるようにし、リスポーンカウントを増やす
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

	// 結果ターンになると、カウント済みでなくする
	if (tm->GetNowTurn() == TurnManager::TurnState::SelectResultTurn)
	{
		if (_isCounted)
		{
			_isCounted = false;
		}
	}

	// 左クリックをしたときに、マップ内ならば地形情報を取得し、クリックした場所に自軍拠点があればユニット生成メニューを出す
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

	// アクションメニューが開いているときに、各タブを左クリックでユニットを生成し、右クリックでキャンセルする
	if (_actionFlag && !um->IsTargetSet())
	{
		if (Mouse::IsTrigger(MOUSE_INPUT_LEFT) && tm->GetNowTurn() == TurnManager::TurnState::PlayerSelectTurn)
		{
			if (_mousePosX >= ACTION_X && _mousePosY >= ACTION_Y && _mousePosX <= ACTION_X + ACTION_WIDTH && _mousePosY <= ACTION_Y + (ACTION_HEIGHT / 3))
			{
				unm->_unitList.push_back(SpawnUnit(UnitType::Soldier, rs));
				_actionFlag = false;
				_reSpawnCount = 0;
			}
			else if (_mousePosX >= ACTION_X && _mousePosY >= ACTION_Y + (ACTION_HEIGHT / 3) && _mousePosX <= ACTION_X + ACTION_WIDTH && _mousePosY <= ACTION_Y + (2 * ACTION_HEIGHT / 3))
			{
				unm->_unitList.push_back(SpawnUnit(UnitType::Archer, rs));
				_actionFlag = false;
				_reSpawnCount = 0;
			}
			else if (_mousePosX >= ACTION_X && _mousePosY >= ACTION_Y + (2 * ACTION_HEIGHT / 3) && _mousePosX <= ACTION_X + ACTION_WIDTH && _mousePosY <= ACTION_Y + (3 * ACTION_HEIGHT / 3))
			{
				unm->_unitList.push_back(SpawnUnit(UnitType::Scout, rs));
				_actionFlag = false;
				_reSpawnCount = 0;
			}
		}
		else if (Mouse::IsTrigger(MOUSE_INPUT_RIGHT))
		{
			_actionFlag = false;
		}
	}

	// 4ターンごとに敵ユニットをスポーンさせる
	if (tm->GetTurnCount() % 4 == 0 && !_isEneSpawn && tm->GetNowTurn() == TurnManager::TurnState::EnemyTurn)
	{
		int unitType = GetRand(2);

		if (unitType == 0)
			unm->_unitList.push_back(SpawnEnemyUnit(UnitType::Soldier, rs));
		else if (unitType == 1)
			unm->_unitList.push_back(SpawnEnemyUnit(UnitType::Archer, rs));
		else if (unitType == 2)
			unm->_unitList.push_back(SpawnEnemyUnit(UnitType::Scout, rs));

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
	DrawSpawnSpan();
}

// 自軍のユニットを生成する関数
_unitBase::UnitData* BaseManager::SpawnUnit(UnitType unit, RouteSearch* rs)
{
	_unitBase::UnitData* unitTemp = new _unitBase::UnitData;

	Vec2 posInd = Vec2(MY_BASE_X, MY_BASE_Y);

	unitTemp->destPos = Vec2(ENEMY_BASE_X, ENEMY_BASE_Y);
	rs->RouteSearchAstar(posInd, rs->_moveCount, unitTemp->destPos);

	unitTemp->name = "自軍追加部隊";
	unitTemp->pos = posInd;
	unitTemp->type = unit;
	_unitBase::SetStatusByType(*unitTemp);
	unitTemp->color = color::YellowColor;
	unitTemp->moveTimer = 0;
	unitTemp->isEnemy = false;
	unitTemp->moveRoute = rs->GetRouteList(posInd, unitTemp->destPos);
	unitTemp->routeIndex = 0;
	unitTemp->state = UnitState::Move;
	unitTemp->isHeapAllocated = true;

	return unitTemp;
}

// 敵軍のユニットを生成する関数
_unitBase::UnitData* BaseManager::SpawnEnemyUnit(UnitType unit, RouteSearch* rs)
{
	_unitBase::UnitData* unitTemp = new _unitBase::UnitData;

	Vec2 posInd = Vec2(ENEMY_BASE_X, ENEMY_BASE_Y);

	unitTemp->destPos = Vec2(MY_BASE_X, MY_BASE_Y);
	rs->RouteSearchAstar(posInd, rs->_moveCount, unitTemp->destPos);

	unitTemp->name = "敵軍追加部隊";
	unitTemp->pos = posInd;
	unitTemp->type = unit;
	_unitBase::SetStatusByType(*unitTemp);
	unitTemp->color = color::RedColor;
	unitTemp->moveTimer = 0;
	unitTemp->isEnemy = true;
	unitTemp->moveRoute = rs->GetRouteList(posInd, unitTemp->destPos);
	unitTemp->routeIndex = 0;
	unitTemp->state = UnitState::Move;
	unitTemp->isHeapAllocated = true;

	return unitTemp;
}

// スポーン可能までのターン数を表示する関数
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