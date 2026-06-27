#pragma once
#include"_unitBase.h"
#include"RouteSearch.h"

class PlayerUnit : public _unitBase
{
public:
	PlayerUnit();
	~PlayerUnit();
	void Init(RouteSearch* rs) override;
	void Update() override;
	void Draw() override;

private:
	// 主部隊のピクセル座標
	Vec2 _mainPosPixel;
	// 副部隊のピクセル座標
	Vec2 _subPosPixel;
	// 主部隊のノード座標
	Vec2 _mainPosInd;
	// 副部隊のノード座標
	Vec2 _subPosInd;
};