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
	void GenerateMissile(VECTOR pos, VECTOR front, bool isEnemy, const std::shared_ptr<MagicManager>& pManager);
	// 消費魔力のゲッター
	int GetUseMp() const { return _magicMissile.useMp; }
	// マジックミサイルのハンドル
	void SetMagicMissileH(int handle) { _magicMissile.effectResourceH = handle; }
private:
	// 目標を設定
	void SetTarget(VECTOR target) { _magicMissile.moveDirection = target; }

private:
	MagicData _magicMissile;
};