#pragma once
#include "Bases/MagicBase.h"
#include <memory>

class MagicManager;

class MagicBeam : public MagicBase
{
public:
	MagicBeam();
	virtual ~MagicBeam() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;
	// ビームを生成
	void GenerateBeam(VECTOR pos, VECTOR front, bool isEnemy, std::shared_ptr<MagicManager> pManager);

private:
	MagicData _magicBeam;
};
