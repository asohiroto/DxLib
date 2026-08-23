#pragma once
#include "Bases/MagicBase.h"
#include <memory>

class MagicManager;

class MagicShot : public MagicBase
{
public:
	MagicShot();
	virtual ~MagicShot() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;
	/// <summary>
	/// 魔法を生成する
	/// </summary>
	/// <param name="pos">生成するユニットの位置</param>
	/// <param name = "front"> 生成するユニットの正面ベクトル</param>
	/// <param name = "isEnemy">敵が生成したか</param>
	void GenerateShot(VECTOR pos, VECTOR front, bool isEnemy, std::shared_ptr<MagicManager> pManager);
	// マジックショットが存在するか
	bool IsExist() const { return _magicShot.isExist; }
	// 消費魔力のゲッター
	int GetUseMp() const { return _magicShot.useMp; }
	// エフェクトのハンドルのセッター
	void SetMagicShotH(int handle) { _magicShot.effectResourceH = handle; }
private:
	// マジックショット
	MagicData _magicShot;
};