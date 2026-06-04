#include"DxLib.h"
#include"Character.h"
#include"Castle.h"

Castle::Castle()
{
}

Castle::~Castle()
{
	// 読み込んだ画像のグラフィックハンドルを削除
	DeleteGraph(units[0].handle);
}

void Castle::Init()
{
	m_pos = Vec2(1600.0f, 950.0f); // 城の位置

	// ------プレイヤー部隊のデータ設定------
	UnitData castle;
	castle.name = "城";
	castle.target = "無し";
	castle.soldierCount = 1500;
	castle.attack = 0;
	castle.speed = 0;
	castle.attackDistance = 0;
	castle.attackRange = 0;
	castle.handle = LoadGraph("media/castle.png");
	castle.isEnemy = true;
	units.push_back(castle);
}

void Castle::Update()
{

}

void Castle::Draw()
{
	// 城の描画
	DrawGraph(m_pos.x - 100.0f, m_pos.y - 150.0f, units[0].handle, TRUE);

	// 頭上にデータの表示
	DrawFormatString(m_pos.x, m_pos.y - 180.0f, GetColor(255, 255, 255), " % s : %d人", units[0].name.c_str(), units[0].soldierCount);

}