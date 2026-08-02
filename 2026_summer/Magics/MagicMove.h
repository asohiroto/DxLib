#pragma once
#pragma once
#include "Bases/MagicBase.h"

class MagicMove
{
public:
	MagicMove();
	virtual ~MagicMove();
	void Init();
	void End();
	void Update();
	void Draw();
	// マジックショットの挙動
	void ShotMove(MagicBase::MagicData& data);

private:
};