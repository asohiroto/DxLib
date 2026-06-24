#pragma once
#include"_unitBase.h"
#include"RouteSearch.h"
#include"EnemyUnit.h"

class PlayerUnit : public _unitBase
{
public:
	PlayerUnit();
	~PlayerUnit();
	void Init() override;
	void Update() override;
	void Draw() override;

private:

	RouteSearch* p_RouteSearch;
	
	// 主部隊
	_unitBase::UnitData _mainUnit;

	// 副部隊
	_unitBase::UnitData _subUnit;

	// 主部隊のピクセル座標
	Vec2 _mainPosPixel;

	// 副部隊のピクセル座標
	Vec2 _subPosPixel;

	// 主部隊のノード座標
	Vec2 _mainPosInd;

	// 副部隊のノード座標
	Vec2 _subPosInd;

	// 主部隊が動く間隔
	int _mainMoveTimer;
	
	// 副部隊が動く間隔
	int _subMoveTimer;

	void StateMove(UnitData& data) override;
	void StateIdle(UnitData& data, int& timer) override;
	void StateArrived(UnitData& data) override;
};