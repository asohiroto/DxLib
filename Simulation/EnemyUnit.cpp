#include"EnemyUnit.h"
#include"_unitBase.h"
#include"DxLib.h"
#include"AsoDxLib/color.h"

EnemyUnit::EnemyUnit() :
	p_RouteSearch(nullptr),
	_mainPosPixel(),
	_mainPosInd(),
	_mainMoveTimer(0),
	_subMoveTimer(0)
{

}

EnemyUnit::~EnemyUnit()
{
	delete p_RouteSearch;
}

void EnemyUnit::Init()
{
	p_RouteSearch = new RouteSearch;

	// 敵の主部隊の初期化処理-------------------------------------------------------

	// 【仮】ユニットの初期位置をランダムに設定
	int x1 = GetRand((GameDefine::NODE_WIDTH / 2) - 1) + (GameDefine::NODE_WIDTH / 2);
	int y1 = GetRand(GameDefine::NODE_HEIGHT - 1);

	_mainPosInd = Vec2(x1, y1);

	_mainUnit.destPos = Vec2(GameDefine::MY_BASE_X, GameDefine::MY_BASE_Y);

	// 経路探索
	p_RouteSearch->RouteSearchAstar(_mainPosInd, p_RouteSearch->_moveCount, _mainUnit.destPos);

	// 主部隊のデータ
	_mainUnit.pos = _mainPosInd;
	_mainUnit.type = UnitType::Soldier;
	_unitBase::SetStatusByType(_mainUnit);
	_mainUnit.color = color::PurpleColor;
	_mainUnit.isEnemy = true;
	_mainUnit.moveRoute = p_RouteSearch->GetRouteList(_mainPosInd, _mainUnit.destPos);
	_mainUnit.routeIndex = 0;
	_mainUnit.state = UnitState::Move;

	//-------------------------------------------------------------------------------

	// 敵の主部隊の初期化処理--------------------------------------------------------

	// 【仮】ユニットの初期位置をランダムに設定
	int x2 = GetRand((GameDefine::NODE_WIDTH / 2) - 1) + (GameDefine::NODE_WIDTH / 2);
	int y2 = GetRand(GameDefine::NODE_HEIGHT - 1);

	_subPosInd = Vec2(x2, y2);

	_subUnit.destPos = Vec2(GameDefine::MY_BASE_X, GameDefine::MY_BASE_Y);

	// 経路探索
	p_RouteSearch->RouteSearchAstar(_subPosInd, p_RouteSearch->_moveCount, _subUnit.destPos);

	// 副部隊のデータ
	_subUnit.pos = _subPosInd;
	_subUnit.type = UnitType::Archer;
	_unitBase::SetStatusByType(_subUnit);
	_subUnit.color = color::PinkColor;
	_subUnit.isEnemy = true;
	_subUnit.moveRoute = p_RouteSearch->GetRouteList(_subPosInd, _subUnit.destPos);
	_subUnit.routeIndex = 0;
	_subUnit.state = UnitState::Move;

	//-------------------------------------------------------------------------------
}

void EnemyUnit::Update()
{
	_mainMoveTimer++;
	_subMoveTimer++;

	if (_mainMoveTimer > 10)
		SetMoveByState(_mainUnit, _mainMoveTimer);

	if (_subMoveTimer > 10)
		SetMoveByState(_subUnit, _subMoveTimer);

	_mainPosPixel = _mainUnit.pos * GameDefine::NODE_SIZE;
	_subPosPixel = _subUnit.pos * GameDefine::NODE_SIZE;
}

void EnemyUnit::Draw()
{
	DrawBox(_mainPosPixel.x, _mainPosPixel.y, _mainPosPixel.x + GameDefine::NODE_SIZE, _mainPosPixel.y + GameDefine::NODE_SIZE, _mainUnit.color, true);
	DrawBox(_subPosPixel.x, _subPosPixel.y, _subPosPixel.x + GameDefine::NODE_SIZE, _subPosPixel.y + GameDefine::NODE_SIZE, _subUnit.color, true);

}

void EnemyUnit::StateMove(UnitData& data)
{
	if (data.moveRoute.empty() || data.routeIndex >= (int)data.moveRoute.size())
	{
		data.state = UnitState::Arrived;
		return;
	}

	data.pos = data.moveRoute[data.routeIndex];

	data.state = UnitState::Idle;
}

void EnemyUnit::StateIdle(UnitData& data, int& timer)
{
	if (data.pos.x == GameDefine::ENEMY_BASE_X && data.pos.y == GameDefine::ENEMY_BASE_Y) data.state = UnitState::Arrived;

	data.routeIndex++;
	timer = 0;
	data.state = UnitState::Move;
}

void EnemyUnit::StateArrived(UnitData& data)
{
	
}
