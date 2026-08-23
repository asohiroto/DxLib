#pragma once
#include "Bases/MagicBase.h"
#include <memory>

class MagicManager;

class MagicFury : public MagicBase
{
public:
	MagicFury();
	virtual ~MagicFury() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;
	// フューリーを生成
	void GenerateFury(VECTOR pos, VECTOR front, bool isEnemy, std::shared_ptr<MagicManager> pManager);
	// マジックフューリーのセッター
	void SetMagicFuryH(int handle) { _magicFury.effectResourceH = handle; }
private:
	MagicData _magicFury;
};