#pragma once
#include"AsoDxLib/Vec2.h"
#include"AsoDxLib/Mouse.h"
#include"_unitBase.h"
#include"PlayerUnit.h"
#include"EnemyUnit.h"

class UIManager
{
public:
	UIManager();
	~UIManager();
	void Init();
	void Update();
	void Draw();

private:
	// クリック位置のピクセル座標をノード座標に変換する関数
	Vec2 ChangePixelToIndex(Vec2 mousePos);
	// クリック位置からユニットデータを取得する関数
	_unitBase::UnitData* GetUnitDataFromPos(Vec2 mousePos);
	// ユニットデータを描画する関数
	void DrawUnitData(_unitBase::UnitData data);

private:
	PlayerUnit* p_PlayerUnit;
	EnemyUnit* p_EnemyUnit;
};