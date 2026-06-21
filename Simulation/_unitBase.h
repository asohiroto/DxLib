#pragma once
#include"AsoDxLib/Vec2.h"
#include"UnitState.h"
#include"UnitType.h"
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
		int color;
		// 建築可能か
		bool canArchitect;
		// 敵か味方か
		bool isEnemy;
		// ユニットの状態
		UnitState state;
		// 兵科
		UnitType type;
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

protected:
	// 兵科ごとに能力を変える関数
	void SetStatusByType(UnitData& data)
	{
		switch (data.type)
		{
		case UnitType::Soldier:
			data.hp = 100;
			data.attack = 30;
			data.attackRange = 1;
			data.canArchitect = false;
			return;

		case UnitType::Archer:
			data.hp = 60;
			data.attack = 20;
			data.attackRange = 3;
			data.canArchitect = false;
			return;

		case UnitType::Engineer:
			data.hp = 30;
			data.attack = 10;
			data.attackRange = 1;
			data.canArchitect = true;
			return;

		default:
			printfDx("Warning: Unknown UnitType\n");
			break;
		}
	}

};