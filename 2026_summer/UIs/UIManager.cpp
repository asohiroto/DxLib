#include "UIManager.h"
#include "EnemyHpBar.h"
#include "PlayerHpBar.h"
#include "PlayerMpBar.h"

UIManager::UIManager() :
	p_EneHpBar(nullptr),
	p_PlayHpBar(nullptr),
	p_PlayMpBar(nullptr)
{
}

UIManager::~UIManager()
{
}

void UIManager::Init(float eneMaxHp, float playMaxHp, float playMaxMp)
{
	p_EneHpBar = std::make_shared<EnemyHpBar>();
	p_EneHpBar->Init(eneMaxHp);

	p_PlayHpBar = std::make_shared<PlayerHpBar>();
	p_PlayHpBar->Init(playMaxHp);

	p_PlayMpBar = std::make_shared<PlayerMpBar>();
	p_PlayMpBar->Init(playMaxMp);
}

void UIManager::End()
{
}

void UIManager::Update(float eneNowHp, float playNowHp, float playNowMp)
{
	p_EneHpBar->Update(eneNowHp);
	p_PlayHpBar->Update(playNowHp);
	p_PlayMpBar->Update(playNowMp);
}

void UIManager::Draw()
{
	p_EneHpBar->Draw();
	p_PlayHpBar->Draw();
	p_PlayMpBar->Draw();
}
