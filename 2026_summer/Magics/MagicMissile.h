#pragma once
#pragma once
#include "Bases/MagicBase.h"
#include <memory>

class MagicManager;

class MagicMissile : public MagicBase
{
public:
	MagicMissile();
	virtual ~MagicMissile() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;

	// マジックミサイルを生成 
	void GenerateMissile(VECTOR pos, VECTOR front, bool isEnemy, std::shared_ptr<MagicManager> pManager);
	// 目標を設定
	void SetTarget(VECTOR target) { _magicMissile.moveDirection = target; }
	// 消費魔力のゲッター
	int GetUseMp() const { return _magicMissile.useMp; }
private:
	MagicData _magicMissile;
};