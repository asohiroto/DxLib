#pragma once
#include <memory>

class EnemyHpBar;
class PlayerHpBar;

class UIManager
{
public:
	UIManager();
	virtual ~UIManager();
	void Init(float eneMaxHp, float playMaxHp);
	void End();
	void Update(float eneNowHp, float playNowHp);
	void Draw();

private:
	std::shared_ptr<EnemyHpBar> p_EneHpBar;
	std::shared_ptr<PlayerHpBar> p_PlayHpBar;
};