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

	_mainEnemy.destPos = Vec2(GameDefine::MY_BASE_X, GameDefine::MY_BASE_Y);

	// 経路探索
	p_RouteSearch->RouteSearchAstar(_mainPosInd, p_RouteSearch->_moveCount, _mainEnemy.destPos);

	// 主部隊のデータ
	_mainEnemy.pos = _mainPosInd;
	_mainEnemy.type = UnitType::Soldier;
	_unitBase::SetStatusByType(_mainEnemy);
	_mainEnemy.color = color::PurpleColor;
	_mainEnemy.isEnemy = true;
	_mainEnemy.moveRoute = p_RouteSearch->GetRouteList(_mainPosInd, _mainEnemy.destPos);
	_mainEnemy.routeIndex = 0;
	_mainEnemy.state = UnitState::Move;

	//-------------------------------------------------------------------------------

	// 敵の主部隊の初期化処理--------------------------------------------------------

	// 【仮】ユニットの初期位置をランダムに設定
	int x2 = GetRand((GameDefine::NODE_WIDTH / 2) - 1) + (GameDefine::NODE_WIDTH / 2);
	int y2 = GetRand(GameDefine::NODE_HEIGHT - 1);

	_subPosInd = Vec2(x2, y2);

	_subEnemy.destPos = Vec2(GameDefine::MY_BASE_X, GameDefine::MY_BASE_Y);

	// 経路探索
	p_RouteSearch->RouteSearchAstar(_subPosInd, p_RouteSearch->_moveCount, _subEnemy.destPos);

	// 副部隊のデータ
	_subEnemy.pos = _subPosInd;
	_subEnemy.type = UnitType::Archer;
	_unitBase::SetStatusByType(_subEnemy);
	_subEnemy.color = color::PinkColor;
	_subEnemy.isEnemy = true;
	_subEnemy.moveRoute = p_RouteSearch->GetRouteList(_subPosInd, _subEnemy.destPos);
	_subEnemy.routeIndex = 0;
	_subEnemy.state = UnitState::Move;

	//-------------------------------------------------------------------------------
}

void EnemyUnit::Update()
{
	_mainMoveTimer++;
	_subMoveTimer++;

	if (_mainMoveTimer > 10)
		SetMoveByState(_mainEnemy, _mainMoveTimer);

	if (_subMoveTimer > 10)
		SetMoveByState(_subEnemy, _subMoveTimer);

	_mainPosPixel = _mainEnemy.pos * GameDefine::NODE_SIZE;
	_subPosPixel = _subEnemy.pos * GameDefine::NODE_SIZE;
}

void EnemyUnit::Draw()
{
	DrawBox(_mainPosPixel.x, _mainPosPixel.y, _mainPosPixel.x + GameDefine::NODE_SIZE, _mainPosPixel.y + GameDefine::NODE_SIZE, _mainEnemy.color, true);
	DrawBox(_subPosPixel.x, _subPosPixel.y, _subPosPixel.x + GameDefine::NODE_SIZE, _subPosPixel.y + GameDefine::NODE_SIZE, _subEnemy.color, true);

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
