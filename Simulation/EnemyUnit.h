#pragma once
#include"_unitBase.h"

class RouteSearch;

class EnemyUnit : public _unitBase
{
public:
	EnemyUnit();
	~EnemyUnit();
	void Init(RouteSearch* rs) override;
	void Update() override;
	void Draw() override;

private:
	// 敵の主部隊のピクセル座標
	Vec2 _mainPosPixel;
	// 敵の主部隊のノード座標
	Vec2 _mainPosInd;
	// 敵の副部隊のピクセル座標
	Vec2 _subPosPixel;
	// 敵の副部隊のノード座標
	Vec2 _subPosInd;
};