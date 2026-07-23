#include "UIManager.h"
#include"Player.h"
#include"GameDefine.h"

using namespace GameDefine;

UIManager::UIManager() :
	_playerDispHp(1000),
	_subDispHp(1000)
{
}

UIManager::~UIManager()
{
}

void UIManager::Init()
{
}

void UIManager::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Player> pPlayerSub)
{
	_playerDispHp += static_cast<float>(pPlayer->GetHp() - _playerDispHp) * HP_LERP_RATE;
	_subDispHp += static_cast<float>(pPlayerSub->GetHp() - _subDispHp) * HP_LERP_RATE;
}

void UIManager::Draw(std::shared_ptr<Player> pPlayer, std::shared_ptr<Player> pPlayerSub)
{
	DrawHpGauge(1, pPlayer);
	DrawHpGauge(2, pPlayerSub);
}

void UIManager::DrawHpGauge(int playerType, std::shared_ptr<Player> pPlayer)
{
	int gaugePosX;
	int gaugePosY = 30;

	int maxGaugeWidth = WIDTH / 2 - 20;
	int gaugeHeight = 30;

	float targetedHp;

	if (playerType == 1)
	{
		gaugePosX = 7;
		targetedHp = _playerDispHp;
	}
	else
	{
		gaugePosX = (WIDTH / 2) + 10;
		targetedHp = _subDispHp;
	}

	float hpRatio = targetedHp / static_cast<float>(pPlayer->GetMaxHp());

	DrawBox(gaugePosX, gaugePosY, gaugePosX + maxGaugeWidth, gaugePosY + gaugeHeight, 0x000000, true);
	
	if (playerType == 1)
	{
		DrawBox(gaugePosX + (maxGaugeWidth * (1.0f - hpRatio)), gaugePosY, gaugePosX + maxGaugeWidth, gaugePosY + gaugeHeight, 0x00ff00, true);
	}
	else
	{
		DrawBox(gaugePosX , gaugePosY, gaugePosX + (maxGaugeWidth * hpRatio), gaugePosY + gaugeHeight, 0x00ff00, true);
	}

	DrawBox(gaugePosX, gaugePosY, gaugePosX + maxGaugeWidth, gaugePosY + gaugeHeight, 0xaaaaaa, false, 3);
}