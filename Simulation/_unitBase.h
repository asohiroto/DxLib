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
		// 攻撃力
		int attack = 0;
		// 攻撃範囲
		int attackRange = 0;
		// スタミナ
		int stamina;
		// 最大スタミナ
		int maxStamina;
		// 見た目
		int color = 0;
		// 行動間隔カウンタ
		int moveTimer = 0;
		// 建築可能か
		bool canBuilding = false;
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
		int routeIndex;
	};

public:
	_unitBase() = default;
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

protected:
	// 兵科ごとに能力を変える関数
	void SetStatusByType(UnitData& data)
	{
		switch (data.type)
		{
		case UnitType::Soldier:
			data.typeName = "Soldier";
			data.hp = 100;
			data.attack = 25;
			data.attackRange = 1;
			data.stamina = 10;
			data.maxStamina = data.stamina;
			data.canBuilding = false;
			return;

		case UnitType::Archer:
			data.typeName = "archer";
			data.hp = 60;
			data.attack = 30;
			data.attackRange = 2;
			data.stamina = 12;
			data.maxStamina = data.stamina;
			data.canBuilding = false;
			return;

		case UnitType::Engineer:
			data.typeName = "Engineer";
			data.hp = 30;
			data.attack = 5;
			data.attackRange = 1;
			data.stamina = 7;
			data.maxStamina = data.stamina;
			data.canBuilding = true;
			return;

		default:
			printfDx("Warning: Unknown UnitType");
			break;
		}
	}
	// 兵科を表示する関数
	void DrawType(UnitData& data, int color)
	{
		std::string typeInit;

		switch (data.type)
		{
		case UnitType::Soldier:
			typeInit = "歩";
			break;
		case UnitType::Archer:
			typeInit = "弓";
			break;
		case UnitType::Engineer:
			typeInit = "工";
			break;
		default:
			typeInit = "?";
			break;
		}
		DrawString(data.pos.x * GameDefine::NODE_SIZE, data.pos.y * GameDefine::NODE_SIZE, typeInit.c_str(), color);
	}

protected:
	// 主部隊
	UnitData _mainUnit;
	// 副部隊
	UnitData _subUnit;
};