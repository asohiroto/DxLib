#pragma once
#include"_unitBase.h"

class PlayerUnit : public _unitBase
{
public:
	PlayerUnit();

	~PlayerUnit() override;

	void Init() override;

	void Update() override;

	void Draw() override;

private:
	_unitBase::UnitData _mainUnit;
};