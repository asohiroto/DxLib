#include "UIManager.h"
#include "EnemyHpBar.h"
#include "PlayerHpBar.h"

UIManager::UIManager() :
	p_EneHpBar(nullptr),
	p_PlayHpBar(nullptr)
{
}

UIManager::~UIManager()
{
}

void UIManager::Init(float eneMaxHp, float playMaxHp)
{
	p_EneHpBar = std::make_shared<EnemyHpBar>();
	p_EneHpBar->Init(eneMaxHp);

	p_PlayHpBar = std::make_shared<PlayerHpBar>();
	p_PlayHpBar->Init(playMaxHp);
}

void UIManager::End()
{
}

void UIManager::Update(float eneNowHp, float playNowHp)
{
	p_EneHpBar->Update(eneNowHp);
	p_PlayHpBar->Update(playNowHp);
}

void UIManager::Draw()
{
	p_EneHpBar->Draw();
	p_PlayHpBar->Draw();
}
