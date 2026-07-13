#pragma once
#include"GameDefine.h"
#include"_unitBase.h"

class RouteSearch;
class UIManager;
class UnitManager;
class TurnManager;

class BaseManager
{
public:
	BaseManager();
	~BaseManager();
	void Init();
	void Update(RouteSearch* rs, UIManager* um, UnitManager* unm, TurnManager* tm);
	void Draw(UIManager* um);
public:

private:
	// ユニットを生産する関数
	_unitBase::UnitData* SpawnUnit(UnitType unit, RouteSearch* rs);
	// 敵ユニットを生産する関数
	_unitBase::UnitData* SpawnEnemyUnit(UnitType unit, RouteSearch* rs);
	// 引数のユニットのステータスを、指定のクラスのものに変更する関数
	void ChangeStatusByType(UnitType unit, _unitBase::UnitData& data);
	// 兵科を表示する関数
	//void DrawType(_unitBase::UnitData* data, int color);
private:
	// マウスの位置
	int _mousePosX, _mousePosY;
	// 選択したマスの地形保存用
	TileType _tileTemp;
	// アクションメニューを開くか
	bool _actionFlag = false;
	// 生成するユニットの保存用
	std::vector<_unitBase::UnitData*> _unitTemp;
	// 敵ユニットを生成したか
	bool _isEneSpawn = false;
	// スポーンしてからのカウント
	int _reSpawnCount = 4;
	// カウントを増やしたか
	bool _isCounted = false;
};