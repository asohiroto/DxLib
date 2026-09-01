#include "UIManager.h"
#include "EnemyHpBar.h"
#include "PlayerHpBar.h"
#include "PlayerMpBar.h"
#include "PlayerUltGauge.h"

UIManager::UIManager() :
	p_EneHpBar(nullptr),
	p_PlayHpBar(nullptr),
	p_PlayMpBar(nullptr),
	p_PlayUltGauge(nullptr)
{
}

UIManager::~UIManager()
{
}

void UIManager::Init(float eneMaxHp, float playMaxHp,
	float playMaxMp, float maxCharge, int pHpH, int eHpH, int ultH)
{
	p_EneHpBar = std::make_shared<EnemyHpBar>();
	p_EneHpBar->Init(eneMaxHp, eHpH);

	p_PlayHpBar = std::make_shared<PlayerHpBar>();
	p_PlayHpBar->Init(playMaxHp, pHpH);

	p_PlayMpBar = std::make_shared<PlayerMpBar>();
	p_PlayMpBar->Init(playMaxMp);

	p_PlayUltGauge = std::make_shared<PlayerUltGauge>();
	p_PlayUltGauge->Init(maxCharge, ultH);
}

void UIManager::End()
{
}

void UIManager::Update(float eneNowHp, float playNowHp, float playNowMp, float nowCharge)
{
	p_EneHpBar->Update(eneNowHp);
	p_PlayHpBar->Update(playNowHp);
	p_PlayMpBar->Update(playNowMp);
	p_PlayUltGauge->Update(nowCharge);
}

void UIManager::Draw()
{
	p_EneHpBar->Draw();
	p_PlayHpBar->Draw();
	//p_PlayMpBar->Draw();
	p_PlayUltGauge->Draw();
}
