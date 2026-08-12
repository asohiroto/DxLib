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
	// マジックミサイルの挙動
	void MissileMove(MagicBase::MagicData& data, VECTOR targetPos);
	// マジックビームの挙動
	void BeamMove(MagicBase::MagicData& data, VECTOR targetPos);
};
