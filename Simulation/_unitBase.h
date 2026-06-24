#pragma once
#include"AsoDxLib/Vec2.h"
#include"UnitState.h"
#include"UnitType.h"
#include"DxLib.h"
#include<vector>

class _unitBase
{
public:

	// ユニットの持つ情報
	struct UnitData
	{
		// 現在位置
		Vec2 pos = Vec2(0, 0);
		// 目的地
		Vec2 destPos = Vec2(0,0);
		// 体力
		int hp = 0;
		// 攻撃力
		int attack = 0;
		// 攻撃範囲
		int attackRange = 0;
		// 見た目
		int color = 0;
		// 建築可能か
		bool canArchitect = false;
		// 敵か味方か
		bool isEnemy = false;
		// ユニットの状態
		UnitState state = UnitState::Idle;
		// 兵科
		UnitType type = UnitType::Soldier;
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
			printfDx("Warning: Unknown UnitType");
			break;
		}
	}

	// 状態ごとに行動を変える関数
	void SetMoveByState(UnitData& data, int& timer)
	{
		switch (data.state)
		{
		case UnitState::Move:
			StateMove(data);
			break;

		case UnitState::Arrived:
			StateArrived(data);
			break;

		case UnitState::Idle:
			StateIdle(data, timer);
			break;

		default:
			break;
		}
	}

	// ルート探索に基づく移動処理
	virtual void StateMove(UnitData& data) = 0;

	// 待機処理
	virtual void StateIdle(UnitData& data, int& timer) = 0;

	// 到着処理
	virtual void StateArrived(UnitData& data) = 0;
};