#include"PlayerUnit.h"
#include"_unitBase.h"
#include"DxLib.h"
#include"GameDefine.h"
#include"AsoDxLib/LibIncluder.h"

PlayerUnit::PlayerUnit()
{

}

PlayerUnit::~PlayerUnit()
{

}

void PlayerUnit::Init()
{
	// 【仮】ユニットの初期位置をランダムに設定
	int x = GetRand(GameDefine::NODE_WIDTH - 1);
	int y = GetRand(GameDefine::NODE_HEIGHT - 1);

	// 主軍部隊のデータ
	_mainUnit =
	{
		_mainUnit.pos = Vec2(x * GameDefine::NODE_SIZE, y * GameDefine::NODE_SIZE),
		_mainUnit.destPos = Vec2(GameDefine::ENEMY_BASE_X, GameDefine::ENEMY_BASE_Y),
		_mainUnit.hp = 100, _mainUnit.attack = 50, _mainUnit.attackRange = 1,
		_mainUnit.canArchitect = false, _mainUnit.isEnemy = false,
		_mainUnit.moveRoute = 
	};
}

void PlayerUnit::Update()
{

}

void PlayerUnit::Draw()
{
	DrawBox(_mainUnit.pos.x, _mainUnit.pos.y, _mainUnit.pos.x + GameDefine::NODE_SIZE, _mainUnit.pos.y + GameDefine::NODE_SIZE, color::YellowColor, true);
}