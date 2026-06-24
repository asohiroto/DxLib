#pragma once
#include"_unitBase.h"
#include"RouteSearch.h"

class EnemyUnit : public _unitBase
{
public:
	EnemyUnit();
	~EnemyUnit();
	void Init() override;
	void Update() override;
	void Draw() override;

private:

	RouteSearch* p_RouteSearch;

	// 敵の主部隊のピクセル座標
	Vec2 _mainPosPixel;

	// 敵の主部隊のノード座標
	Vec2 _mainPosInd;

	// 敵の副部隊のピクセル座標
	Vec2 _subPosPixel;

	// 敵の副部隊のノード座標
	Vec2 _subPosInd;

	// 敵の主部隊が動く間隔
	int _mainMoveTimer;

	// 敵の副部隊が動く間隔
	int _subMoveTimer;

	void StateMove(UnitData& data) override;
	void StateIdle(UnitData& data, int& timer) override;
	void StateArrived(UnitData& data) override;
	//void StateAttack(UnitData& data) override;
	//void StateDead(UnitData& data) override;
};