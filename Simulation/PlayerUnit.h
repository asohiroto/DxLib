#pragma once
#include"Unit.h"

class PlayerUnit : public Unit
{
public:
	PlayerUnit();

	~PlayerUnit() override;

	void Init() override;

	void Update() override;

	void Draw() override;

};