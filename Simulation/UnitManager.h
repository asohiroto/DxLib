#pragma once
#include"PlayerUnit.h"
#include"EnemyUnit.h"
#include"RouteSearch.h"
#include"TurnManager.h"
#include"_unitBase.h"

class UnitManager
{
public:
	UnitManager();
	~UnitManager();
	void Init(RouteSearch* rs);
	void Update(RouteSearch* rs, TurnManager* tm);
	void Draw(TurnManager* tm);

	// 移動処理
	void StateMove(_unitBase::UnitData& data, int& timer, RouteSearch* rs);
	// 待機処理
	void StateIdle(_unitBase::UnitData& data, int& timer, TurnManager* tm);
	// 到着処理
	void StateArrived(_unitBase::UnitData& data, int& timer);
	// 攻撃処理
	void StateAttack(_unitBase::UnitData& data, int& timer, RouteSearch* rs);
	// 壊滅処理
	void StateDead(_unitBase::UnitData& data, int& timer);
	// ２ユニット間のマンハッタン距離を返す関数
	int Distance(_unitBase::UnitData* player, _unitBase::UnitData* enemy);
	// 状態ごとに行動を変える関数
	void SetMoveByState(_unitBase::UnitData& data, int& timer, RouteSearch* rs, TurnManager* tm);
	// プレイヤーユニットクラスのゲッター
	PlayerUnit* GetPlayerPointer() { return p_PlayerUnit; }
	// エネミーユニットクラスのゲッター
	EnemyUnit* GetEnemyPointer() { return p_EnemyUnit; }

public:
	// 全ユニットを管理する可変長リスト
	std::vector<_unitBase::UnitData*> _unitList;

private:
	PlayerUnit* p_PlayerUnit;
	EnemyUnit* p_EnemyUnit;

	// 何ユニットが行動終了したか
	int _finishCount;
	// 予約済みグリッド
	bool _occupiedMap[GameDefine::NODE_HEIGHT][GameDefine::NODE_WIDTH];
	// マウスの位置
	int _mousePosX, _mousePosY;
	// 現在存在する味方ユニットの数
	int _playerCount;
	// 現在存在する敵ユニットの数
	int _enemyCount;
	// 現在の拠点の体力
	int _myBaseHpNow, _enemyBaseHpNow;
	// 拠点の最大体力
	int _myBaseHpMax, _enemyBaseHpMax;
	// 拠点の攻撃力
	int _myBaseAttack, _enemyBaseAttack;
	// 次に移動するグリッド
	Vec2 _nextPos;
};