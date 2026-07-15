#pragma once
#include"AsoDxLib/Vec2.h"
#include"AsoDxLib/Color.h"
#include"UnitState.h"
#include"UnitType.h"
#include"RouteSearch.h"
#include"DxLib.h"
#include<vector>
#include<string>

class _unitBase
{
public:
	// ユニットの持つ情報
	struct UnitData
	{
		// 部隊名
		std::string name;
		// 兵科名
		std::string typeName;
		// 現在位置
		Vec2 pos = Vec2(0, 0);
		// 目的地
		Vec2 destPos = Vec2(0, 0);
		// 体力
		int hp = 0;

		int maxHp = 0;
		// 攻撃力
		int attack = 0;
		// 攻撃範囲
		int attackRange = 0;
		// スタミナ
		int stamina = 0;
		// 最大スタミナ
		int maxStamina = 0;
		// 見た目
		int color = 0;
		// 行動間隔カウンタ
		int moveTimer = 0;
		// 敵か味方か
		bool isEnemy = false;
		// 攻撃済みか
		bool hasAttacked = false;
		// ユニットの状態
		UnitState state = UnitState::Idle;
		// 兵科
		UnitType type = UnitType::Soldier;
		// 経路探索関係-------
		// ユニットの進む経路
		std::vector<Vec2> moveRoute;
		// ルートの中の位置
		int routeIndex = 0;
		// ヒープ領域に確保されているか
		bool isHeapAllocated = false;
	};

public:
	_unitBase() {};
	virtual ~_unitBase() {};
	virtual void Init(RouteSearch* rs) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	// ユニットデータのゲッター
	UnitData& GetMainUnit() { return _mainUnit; }
	UnitData& GetSubUnit() { return _subUnit; }
	// 各ユニットの座標のゲッター
	Vec2 GetPosMain() { return _mainUnit.pos; }
	Vec2 GetPosSub() { return _subUnit.pos; }

	// 兵科ごとに能力を変える関数
	static void SetStatusByType(UnitData& data)
	{
		switch (data.type)
		{
		case UnitType::Soldier:
			data.typeName = "Soldier";
			data.hp = 100;
			data.maxHp = data.hp;
			data.attack = 25;
			data.attackRange = 1;
			data.stamina = 10;
			data.maxStamina = data.stamina;
			return;

		case UnitType::Archer:
			data.typeName = "Archer";
			data.hp = 60;
			data.maxHp = data.hp;
			data.attack = 30;
			data.attackRange = 2;
			data.stamina = 12;
			data.maxStamina = data.stamina;
			return;

		case UnitType::Scout:
			data.typeName = "Scout";
			data.hp = 40;
			data.maxHp = data.hp;
			data.attack = 15;
			data.attackRange = 1;
			data.stamina = 20;
			data.maxStamina = data.stamina;
			return;

		default:
			printfDx("Warning: Unknown UnitType");
			break;
		}
	}

protected:
	// 主部隊
	UnitData _mainUnit;
	// 副部隊
	UnitData _subUnit;
};