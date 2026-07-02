#pragma once
#include"AsoDxLib/Vec2.h"
#include"_unitBase.h"
#include"PlayerUnit.h"
#include"EnemyUnit.h"
#include"RouteSearch.h"
#include"TurnManager.h"
#include"UnitManager.h"
#include<string>

class UIManager
{
public:
	UIManager();
	~UIManager();
	void Init();
	void Update(PlayerUnit* pu, EnemyUnit* eu, RouteSearch* rs, TurnManager* tm, UnitManager* unm);
	void Draw(RouteSearch* rs);
	// クリック位置のピクセル座標をノード座標に変換する関数
	Vec2 ChangePixelToIndex(Vec2 mousePos);

private:
	// クリック位置からユニットデータを取得する関数
	_unitBase::UnitData* GetUnitDataFromPos(Vec2 mousePos, PlayerUnit* pu, EnemyUnit* eu, UnitManager* unm) const;
	// ユニットデータを描画する関数
	void DrawUnitData(_unitBase::UnitData* data);
	// 引数のデータをいい感じに描画する関数
	void DrawData(Vec2 pos, std::string name, Vec2 unitPos, std::string type, int hp, int attack);

private:
	// 目標設定中フラグ
	bool _targetSet = false;
	// 目標設定モード用フラグ
	bool _targetSettingMode = false;
	// マウスの位置
	int _mousePosX, _mousePosY;
	// ノード座標系のマウス位置
	Vec2 _nodeIndex;
	// 選択したユニット
	_unitBase::UnitData* _unitTemp;
	// 情報表示するために選択したユニット
	_unitBase::UnitData* _unit;
	// セレクトボタンに表示するテキスト
	std::string _buttonText;

};