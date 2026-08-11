#pragma once
#include "Bases/MagicBase.h"

class MagicWall : public MagicBase
{
public:
	MagicWall();
	virtual ~MagicWall() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;

private:
	MagicData _magicWall;
};