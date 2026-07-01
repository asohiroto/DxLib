#include"UIManager.h"
#include"GameDefine.h"
#include"AsoDxLib/Mouse.h"
#include"AsoDxLib/Color.h"

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
	Mouse::Update();

	// マウスの座標を取得
	GetMousePoint(&_mousePosX, &_mousePosY);
	_nodeIndex = ChangePixelToIndex(Vec2((float)_mousePosX, (float)_mousePosY));

	// ターンがプレイヤー選択ターンの時のみ、マウス入力を受け付ける
	if (tm->GetNowTurn() == TurnManager::TurnState::PlayerSelectTurn)
	{
		// 左クリックでユニットを選択、再度左クリックで目的地を設定
		if (Mouse::IsTrigger(MOUSE_INPUT_LEFT) && !_targetSet)
		{
			if (GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE <= _mousePosX <= GameDefine::NODE_HEIGHT * GameDefine::NODE_SIZE, GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE + 200
				&& GameDefine::NODE_HEIGHT * GameDefine::NODE_SIZE <= _mousePosY <= GameDefine::NODE_HEIGHT * GameDefine::NODE_SIZE + 40)
			{

			}

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

void UIManager::Draw()
{
	DrawFormatString(0, (GameDefine::NODE_HEIGHT + 1) * GameDefine::NODE_SIZE, color::WhiteColor, "%d,  %d", _mousePosX, _mousePosY);
	DrawFormatString(0, (GameDefine::NODE_HEIGHT + 2) * GameDefine::NODE_SIZE, color::WhiteColor, "%f,  %f", _nodeIndex.x, _nodeIndex.y);

	DrawBox(GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE, GameDefine::NODE_HEIGHT * GameDefine::NODE_SIZE, GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE + 200, GameDefine::NODE_HEIGHT * GameDefine::NODE_SIZE + 40, color::OrangeColor, true);
	DrawFormatString(GameDefine::NODE_WIDTH * GameDefine::NODE_SIZE + 65, GameDefine::NODE_HEIGHT * GameDefine::NODE_SIZE + 10, 0x000000, "NextTurn");

	// if(_targetSet) DrawFormatString(0, (GameDefine::NODE_HEIGHT + 2) * GameDefine::NODE_SIZE, color::WhiteColor, )
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