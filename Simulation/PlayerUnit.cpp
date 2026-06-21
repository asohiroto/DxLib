#include"PlayerUnit.h"
#include"EnemyUnit.h"
#include"_unitBase.h"
#include"DxLib.h"
#include"GameDefine.h"
#include"AsoDxLib/Color.h"

PlayerUnit::PlayerUnit() :
	p_RouteSearch(nullptr),
	p_EnemyUnit(nullptr),
	_mainPosPixel(),
	_mainPosInd(),
	_subPosPixel(),
	_subPosInd(),
	_mainMoveTimer(0),
	_subMoveTimer(0)
{

}

PlayerUnit::~PlayerUnit()
{
	delete p_RouteSearch;
	delete p_EnemyUnit;
}

void PlayerUnit::Init()
{
	p_RouteSearch = new RouteSearch;
	p_EnemyUnit = new EnemyUnit;

	// 主部隊の初期化処理---------------------------------------------------------

	// 【仮】ユニットの初期位置をランダムに設定
	int x1 = GetRand((GameDefine::NODE_WIDTH / 2) - 1);
	int y1 = GetRand(GameDefine::NODE_HEIGHT - 1);

	_mainPosInd = Vec2(x1, y1);

	_mainUnit.destPos = Vec2(GameDefine::ENEMY_BASE_X, GameDefine::ENEMY_BASE_Y);

	// 経路探索
	p_RouteSearch->RouteSearchAstar(_mainPosInd, p_RouteSearch->_moveCount, _mainUnit.destPos);

	// 主部隊のデータ
	_mainUnit.pos = _mainPosInd;
	_mainUnit.type = UnitType::Soldier;
	SetStatusByType(_mainUnit);
	_mainUnit.color = color::YellowColor;
	_mainUnit.isEnemy = false;
	_mainUnit.moveRoute = p_RouteSearch->GetRouteList(_mainPosInd, _mainUnit.destPos);
	_mainUnit.routeIndex = 0;

	//----------------------------------------------------------------------------

	// 副部隊の初期化処理---------------------------------------------------------

	// 【仮】ユニットの初期位置をランダムに設定
	int x2 = GetRand((GameDefine::NODE_WIDTH / 2) - 1);
	int y2 = GetRand(GameDefine::NODE_HEIGHT - 1);

	_subPosInd = Vec2(x2, y2);

	_subUnit.destPos = Vec2(GameDefine::ENEMY_BASE_X, GameDefine::ENEMY_BASE_Y);

	// 経路探索
	p_RouteSearch->RouteSearchAstar(_subPosInd, p_RouteSearch->_moveCount, _subUnit.destPos);

	// 主部隊のデータ
	_subUnit.pos = _subPosInd;
	_subUnit.type = UnitType::Archer;
	SetStatusByType(_subUnit);
	_subUnit.color = color::LightGrayColor;
	_subUnit.isEnemy = false;
	_subUnit.moveRoute = p_RouteSearch->GetRouteList(_subPosInd, _subUnit.destPos);
	_subUnit.routeIndex = 0;

	//----------------------------------------------------------------------------
}

void PlayerUnit::Update()
{
	// 行動間隔のタイマー
	_mainMoveTimer++;
	_subMoveTimer++;

	// ルート探索に基づく、移動処理
	if (!_mainUnit.moveRoute.empty() && _mainUnit.routeIndex < _mainUnit.moveRoute.size() && !_mainUnit.isAttacking)
		_mainUnit.pos = _mainUnit.moveRoute[_mainUnit.routeIndex];

	if (!_subUnit.moveRoute.empty() && _subUnit.routeIndex < _subUnit.moveRoute.size() && !_subUnit.isAttacking)
		_subUnit.pos = _subUnit.moveRoute[_subUnit.routeIndex];


	if (_mainMoveTimer > 10 && !_mainUnit.isAttacking)
	{
		_mainUnit.routeIndex++;
		_mainMoveTimer = 0;
	}

	if (_subMoveTimer > 10 && !_subUnit.isAttacking)
	{
		_subUnit.routeIndex++;
		_subMoveTimer = 0;
	}

	if (_mainUnit.pos.x == GameDefine::ENEMY_BASE_X && _mainUnit.pos.y == GameDefine::ENEMY_BASE_Y && !_mainUnit.isGoal)
	{
		printfDx("main toutyaku!\n");
		_mainUnit.isGoal = true;
	}

	if (_subUnit.pos.x == GameDefine::ENEMY_BASE_X && _subUnit.pos.y == GameDefine::ENEMY_BASE_Y && !_subUnit.isGoal)
	{
		printfDx("sub toutyaku!\n");
		_subUnit.isGoal = true;
	}

	_mainPosPixel = _mainUnit.pos * GameDefine::NODE_SIZE;
	_subPosPixel = _subUnit.pos * GameDefine::NODE_SIZE;

}

void PlayerUnit::Draw()
{
	//p_RouteSearch->DrawRoute();
	DrawBox(_mainPosPixel.x, _mainPosPixel.y, _mainPosPixel.x + GameDefine::NODE_SIZE, _mainPosPixel.y + GameDefine::NODE_SIZE, _mainUnit.color, true);
	DrawBox(_subPosPixel.x, _subPosPixel.y, _subPosPixel.x + GameDefine::NODE_SIZE, _subPosPixel.y + GameDefine::NODE_SIZE, _subUnit.color, true);

}