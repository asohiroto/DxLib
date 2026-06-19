#pragma once
#include"AsoDxLib/Vec2.h"
#include<vector>

class _unitBase
{
public:

	// ユニットの持つ情報
	struct UnitData
	{
		// 現在位置
		Vec2 pos;
		// 目的地
		Vec2 destPos;
		// 体力
		int hp;
		// 攻撃力
		int attack;
		// 攻撃範囲
		int attackRange;
		// 見た目
		int handle;
		// 建築可能か
		bool canArchitect;
		// 敵か味方か
		bool isEnemy;
		// 経路探索関係-------
		// ユニットの進む経路
		std::vector<Vec2> moveRoute;
		// ルートの中の位置
		int routeIndex;
	};

	_unitBase() = default;
	virtual ~_unitBase() {};
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};