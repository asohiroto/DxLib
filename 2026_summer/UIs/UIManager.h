#pragma once
#include <memory>

class EnemyHpBar;
class PlayerHpBar;
class PlayerMpBar;

class UIManager
{
public:
	UIManager();
	virtual ~UIManager();
	void Init(float eneMaxHp, float playMaxHp, float playMaxMp);
	void End();
	void Update(float eneNowHp, float playNowHp, float playNowMp);
	void Draw();

private:
	std::shared_ptr<EnemyHpBar> p_EneHpBar;
	std::shared_ptr<PlayerHpBar> p_PlayHpBar;
	std::shared_ptr<PlayerMpBar> p_PlayMpBar;
};