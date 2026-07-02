#include"UIManager.h"
#include"GameDefine.h"
#include"AsoDxLib/Mouse.h"
#include"AsoDxLib/Color.h"
#include<string>

UIManager::UIManager() :
	_targetSet(false),
	_mousePosX(0),
	_mousePosY(0),
	_unitTemp(nullptr)
{

}

UIManager::~UIManager()
{

}

void UIManager::Init()
{

}

void UIManager::Update(PlayerUnit* pu, EnemyUnit* eu, RouteSearch* rs, TurnManager* tm)
{
	// マウスの座標を取得
	GetMousePoint(&_mousePosX, &_mousePosY);
	_nodeIndex = ChangePixelToIndex(Vec2((float)_mousePosX, (float)_mousePosY));

	// ターンがプレイヤー選択ターンの時のみ、マウス入力を受け付ける
	if (tm->GetNowTurn() == TurnManager::TurnState::PlayerSelectTurn)
	{
		if (_mousePosX <= (GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE) && _mousePosY <= (GameDefine::NODE_HEIGHT * GameDefine::NODE_SIZE))
		{
			// 左クリックでユニットを選択、再度左クリックで目的地を設定
			if (Mouse::IsTrigger(MOUSE_INPUT_LEFT) && !_targetSet)
			{
				_unitTemp = GetUnitDataFromPos(_nodeIndex, pu, eu);
				_targetSet = true;
			}
			else if (Mouse::IsTrigger(MOUSE_INPUT_LEFT) && _targetSet)
			{
				if (_unitTemp != nullptr)
				{
					_unitTemp->destPos = _nodeIndex;
					rs->RouteSearchAstar(_unitTemp->pos, rs->_moveCount, _unitTemp->destPos);
					_unitTemp->moveRoute = rs->GetRouteList(_unitTemp->pos, _unitTemp->destPos);
					_unitTemp->routeIndex = 0;
					_targetSet = false;
				}
				// 右クリックで選択を解除
				else
				{
					_targetSet = false;
				}
			}
			else if (Mouse::IsTrigger(MOUSE_INPUT_RIGHT) && _targetSet)
			{
				_targetSet = false;
			}
		}
	}
}

void UIManager::Draw()
{
	DrawFormatString(0, (GameDefine::NODE_HEIGHT + 1) * GameDefine::NODE_SIZE, color::WhiteColor, "%d,  %d", _mousePosX, _mousePosY);
	DrawFormatString(0, (GameDefine::NODE_HEIGHT + 2) * GameDefine::NODE_SIZE, color::WhiteColor, "%f,  %f", _nodeIndex.x, _nodeIndex.y);
	DrawUnitData(_unitTemp);

	if (_targetSet)
	{
		DrawBox(GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE, 400, GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE + 200, 420, color::CyanColor, true);
		DrawString(GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE, 400, "Selecting...", color::BlackColor);
	}
}

Vec2 UIManager::ChangePixelToIndex(Vec2 mousePos)
{
	int mouseIndX = (int)(mousePos.x / GameDefine::NODE_SIZE);
	int mouseIndY = (int)(mousePos.y / GameDefine::NODE_SIZE);

	Vec2 mouseIndex = Vec2((float)mouseIndX, (float)mouseIndY);

	return mouseIndex;
}

_unitBase::UnitData* UIManager::GetUnitDataFromPos(Vec2 mousePos, PlayerUnit* pu, EnemyUnit* eu) const
{
	if (pu->GetPosMain() == mousePos) return &(pu->GetMainUnit());
	else if (pu->GetPosSub() == mousePos) return &(pu->GetSubUnit());
	else if (eu->GetPosMain() == mousePos) return &(eu->GetMainUnit());
	else if (eu->GetPosSub() == mousePos) return &(eu->GetSubUnit());
	else return nullptr;
}

void UIManager::DrawUnitData(_unitBase::UnitData* data)
{
	std::string tempName;
	Vec2 tempPos;
	std::string tempType;
	int tempHp;
	int tempAttack;

	if (data == nullptr)
	{
		tempName = "No Selecting";
		tempPos = Vec2(0, 0);
		tempType = "No Selecting";
		tempHp = 0;
		tempAttack = 0;
	}
	else
	{
		tempName = data->name;
		tempPos = data->destPos;
		tempType = data->typeName;
		tempHp = data->hp;
		tempAttack = data->attack;
	}

	DrawData(Vec2(GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE, 200), tempName, tempPos, tempType, tempHp, tempAttack);

}

void UIManager::DrawData(Vec2 pos, std::string name, Vec2 unitPos, std::string type, int hp, int attack)
{
	DrawBox(pos.x, pos.y, pos.x + 200, pos.y + 100, color::WhiteColor, true);
	DrawFormatString(pos.x + 10, pos.y, color::BlackColor, name.c_str());
	DrawFormatString(pos.x + 10, pos.y + 20, color::BlackColor, "Destination : %d, %d", unitPos.x, unitPos.y);
	DrawFormatString(pos.x + 10, pos.y + 40, color::BlackColor, type.c_str());
	DrawFormatString(pos.x + 10, pos.y + 60, color::BlackColor, "Hp : %d", hp);
	DrawFormatString(pos.x + 10, pos.y + 80, color::BlackColor, "AttackDamage : %d", attack);
}