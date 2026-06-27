#pragma once
#include"PlayerUnit.h"
#include"EnemyUnit.h"
#include"RouteSearch.h"
#include"_unitBase.h"

class UnitManager
{
public:
	UnitManager();
	~UnitManager();
	void Init(RouteSearch* rs);
	void Update();
	void Draw();
	
	// 移動処理
	void StateMove(_unitBase::UnitData& data, int& timer);
	// 待機処理
	void StateIdle(_unitBase::UnitData& data, int& timer);
	// 到着処理
	void StateArrived(_unitBase::UnitData& data, int& timer);
	// 攻撃処理
	void StateAttack(_unitBase::UnitData& data, int& timer);
	// 壊滅処理
	void StateDead(_unitBase::UnitData& data, int& timer);
	// ２ユニット間のマンハッタン距離を返す関数
	int Distance(_unitBase::UnitData* player, _unitBase::UnitData* enemy);
	// 状態ごとに行動を変える関数
	void SetMoveByState(_unitBase::UnitData& data, int& timer);

public:
	// 全ユニットを管理する可変長リスト
	std::vector<_unitBase::UnitData*> _unitList;

private:
	PlayerUnit* p_PlayerUnit;
	EnemyUnit* p_EnemyUnit;

	bool timeStop = false;
};