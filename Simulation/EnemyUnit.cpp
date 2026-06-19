#include"EnemyUnit.h"
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
	_mainEnemy.hp = 100;
	_mainEnemy.attack = 50;
	_mainEnemy.attackRange = 1;
	_mainEnemy.color = color::PurpleColor;
	_mainEnemy.canArchitect = false;
	_mainEnemy.isEnemy = false;
	_mainEnemy.isAttacking = false;
	_mainEnemy.isGoal = false;
	_mainEnemy.moveRoute = p_RouteSearch->GetRouteList(_mainPosInd, _mainEnemy.destPos);
	_mainEnemy.routeIndex = 0;

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
	_subEnemy.hp = 100;
	_subEnemy.attack = 50;
	_subEnemy.attackRange = 1;
	_subEnemy.color = color::PinkColor;
	_subEnemy.canArchitect = false;
	_subEnemy.isEnemy = false;
	_subEnemy.isAttacking = false;
	_subEnemy.isGoal = false;
	_subEnemy.moveRoute = p_RouteSearch->GetRouteList(_subPosInd, _subEnemy.destPos);
	_subEnemy.routeIndex = 0;

	//-------------------------------------------------------------------------------
}

void EnemyUnit::Update()
{
	_mainMoveTimer++;
	_subMoveTimer++;

	if (!_mainEnemy.moveRoute.empty() && _mainEnemy.routeIndex < _mainEnemy.moveRoute.size() && !_mainEnemy.isAttacking)
		_mainEnemy.pos = _mainEnemy.moveRoute[_mainEnemy.routeIndex];

	if (!_subEnemy.moveRoute.empty() && _subEnemy.routeIndex < _subEnemy.moveRoute.size() && !_subEnemy.isAttacking)
		_subEnemy.pos = _subEnemy.moveRoute[_subEnemy.routeIndex];

	if (_mainMoveTimer > 10 && !_mainEnemy.isAttacking)
	{
		_mainEnemy.routeIndex++;
		_mainMoveTimer = 0;
	}

	if (_subMoveTimer > 10 && !_subEnemy.isAttacking)
	{
		_subEnemy.routeIndex++;
		_subMoveTimer = 0;
	}

	if (_mainEnemy.pos.x == GameDefine::MY_BASE_X && _mainEnemy.pos.y == GameDefine::MY_BASE_Y && !_mainEnemy.isGoal)
	{
		printfDx("main toutyakuda!\n");
		_mainEnemy.isGoal = true;
	}

	if (_subEnemy.pos.x == GameDefine::MY_BASE_X && _subEnemy.pos.y == GameDefine::MY_BASE_Y && !_subEnemy.isGoal)
	{
		printfDx("sub toutyakuda!\n");
		_subEnemy.isGoal = true;
	}

	_mainPosPixel = _mainEnemy.pos * GameDefine::NODE_SIZE;
	_subPosPixel = _subEnemy.pos * GameDefine::NODE_SIZE;
}

void EnemyUnit::Draw()
{
	DrawBox(_mainPosPixel.x, _mainPosPixel.y, _mainPosPixel.x + GameDefine::NODE_SIZE, _mainPosPixel.y + GameDefine::NODE_SIZE, _mainEnemy.color, true);
	DrawBox(_subPosPixel.x, _subPosPixel.y, _subPosPixel.x + GameDefine::NODE_SIZE, _subPosPixel.y + GameDefine::NODE_SIZE, _subEnemy.color, true);

}