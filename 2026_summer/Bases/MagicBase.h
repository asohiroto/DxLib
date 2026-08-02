#pragma once
#include <DxLib.h>

class MagicBase
{
public:
	// 使う魔法の種類
	enum MagicType
	{
		MagicShot,
		MagicMissile,
		MagicWall
	};

	// 魔法のデータ
	struct MagicData
	{
		// 座標
		VECTOR pos;
		// 当たり判定の半径
		float radius;
		// 魔法の進む速度
		float speed;
		// 魔法の種類
		MagicType type;
		// 存在するか
		bool isExist;
		// 敵が使用したモノか
		bool isEnemy;
		// 移動させる方向
		VECTOR moveDirection;
		// 移動距離
		float movedDistance;
	};

public:
	MagicBase() {};
	virtual ~MagicBase() {};
	virtual void Init() {};
	virtual void End() {};
	virtual void Update() {};
	virtual void Draw() {};
};