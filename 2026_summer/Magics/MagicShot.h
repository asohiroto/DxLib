#pragma once
#include "Bases/MagicBase.h"

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
	void GenerateShot(VECTOR pos, VECTOR front, bool isEnemy);
	/// <summary>
	///  魔法を移動させる処理
	/// </summary>
	/// <param name="dir">移動させる方向</param>
	void Move(VECTOR dir);
	// マジックショットが存在するか
	bool IsExist() const { return _magicShot.isExist; }

private:
	// マジックショット
	MagicData _magicShot;
	// 移動距離
	float _movedDistance;
	// 関数の初期操作
	bool _isInitialize;
	// 移動処理用の方向
	VECTOR _distDir;
};