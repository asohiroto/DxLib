#include"UIManager.h"
#include"GameDefine.h"
#include"PlayerUnit.h"
#include"EnemyUnit.h"

UIManager::UIManager() : 
	p_PlayerUnit(nullptr),
	p_EnemyUnit(nullptr)
{

}

UIManager::~UIManager()
{
	delete p_PlayerUnit;
	delete p_EnemyUnit;
}

void UIManager::Init()
{
	p_PlayerUnit = new PlayerUnit;
	p_EnemyUnit = new EnemyUnit;
}

void UIManager::Update()
{

}

void UIManager::Draw()
{

}

Vec2 UIManager::ChangePixelToIndex(Vec2 mousePos)
{
	int mouseIndX = (int)(mousePos.x / GameDefine::NODE_SIZE);
	int mouseIndY = (int)(mousePos.y / GameDefine::NODE_SIZE);

	Vec2 mouseIndex = Vec2((float)mouseIndX, (float)mouseIndY);

	return mouseIndex;
}

_unitBase::UnitData* UIManager::GetUnitDataFromPos(Vec2 mousePos) const
{
	if (p_PlayerUnit->GetPosMain() == mousePos) return &(p_PlayerUnit->GetMainUnit());
	else if (p_PlayerUnit->GetPosSub() == mousePos) return &(p_PlayerUnit->GetSubUnit());
	else if (p_EnemyUnit->GetPosMain() == mousePos) return &(p_EnemyUnit->GetMainUnit());
	else if (p_EnemyUnit->GetPosSub() == mousePos) return &(p_EnemyUnit->GetSubUnit());
	else return nullptr;
}