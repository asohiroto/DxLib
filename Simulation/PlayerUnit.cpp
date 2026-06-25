#include"PlayerUnit.h"
#include"_unitBase.h"
#include"DxLib.h"
#include"GameDefine.h"
#include"AsoDxLib/Color.h"

PlayerUnit::PlayerUnit() :
	_mainPosPixel(),
	_mainPosInd(),
	_subPosPixel(),
	_subPosInd()
{

}

PlayerUnit::~PlayerUnit()
{

}

void PlayerUnit::Init(RouteSearch* rs)
{
	// 主部隊の初期化処理---------------------------------------------------------

	// 【仮】ユニットの初期位置をランダムに設定
	int x1 = GetRand((GameDefine::NODE_WIDTH / 2) - 1);
	int y1 = GetRand(GameDefine::NODE_HEIGHT - 1);

	_mainPosInd = Vec2(x1, y1);

	_mainUnit.destPos = Vec2(GameDefine::ENEMY_BASE_X, GameDefine::ENEMY_BASE_Y);

	// 経路探索
	rs->RouteSearchAstar(_mainPosInd, rs->_moveCount, _mainUnit.destPos);

	// 主部隊のデータ
	_mainUnit.pos = _mainPosInd;
	_mainUnit.type = UnitType::Soldier;
	_unitBase::SetStatusByType(_mainUnit);
	_mainUnit.color = color::YellowColor;
	_mainUnit.moveTimer = 0;
	_mainUnit.isEnemy = false;
	_mainUnit.moveRoute = rs->GetRouteList(_mainPosInd, _mainUnit.destPos);
	_mainUnit.routeIndex = 0;
	_mainUnit.state = UnitState::Move;

	//----------------------------------------------------------------------------

	// 副部隊の初期化処理---------------------------------------------------------

	// 【仮】ユニットの初期位置をランダムに設定
	int x2 = GetRand((GameDefine::NODE_WIDTH / 2) - 1);
	int y2 = GetRand(GameDefine::NODE_HEIGHT - 1);

	_subPosInd = Vec2(x2, y2);

	_subUnit.destPos = Vec2(GameDefine::ENEMY_BASE_X, GameDefine::ENEMY_BASE_Y);

	// 経路探索
	rs->RouteSearchAstar(_subPosInd, rs->_moveCount, _subUnit.destPos);

	// 主部隊のデータ
	_subUnit.pos = _subPosInd;
	_subUnit.type = UnitType::Archer;
	_unitBase::SetStatusByType(_subUnit);
	_subUnit.color = color::LightGrayColor;
	_subUnit.moveTimer = 0;
	_subUnit.isEnemy = false;
	_subUnit.moveRoute = rs->GetRouteList(_subPosInd, _subUnit.destPos);
	_subUnit.routeIndex = 0;
	_subUnit.state = UnitState::Move;

	//----------------------------------------------------------------------------
}

void PlayerUnit::Update()
{
	_mainUnit.moveTimer++;
	_subUnit.moveTimer++;

	_mainPosPixel = _mainUnit.pos * GameDefine::NODE_SIZE;
	_subPosPixel = _subUnit.pos * GameDefine::NODE_SIZE;
}

void PlayerUnit::Draw()
{
	//p_RouteSearch->DrawRoute();
	DrawBox(_mainPosPixel.x, _mainPosPixel.y, _mainPosPixel.x + GameDefine::NODE_SIZE, _mainPosPixel.y + GameDefine::NODE_SIZE, _mainUnit.color, true);
	DrawBox(_subPosPixel.x, _subPosPixel.y, _subPosPixel.x + GameDefine::NODE_SIZE, _subPosPixel.y + GameDefine::NODE_SIZE, _subUnit.color, true);
}



