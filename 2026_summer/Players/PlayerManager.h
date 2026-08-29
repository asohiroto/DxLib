#pragma once
#include "Effects/EffectHandles.h"
#include <memory>

class Player;
class PlayerDodge;
class PlayerMove;

class PlayerManager
{
public:
	PlayerManager();
	virtual ~PlayerManager();
	void Init(int handle, EffectHandles playerMagics);
	void End();
	void Update();
	void Draw();

private:
	std::shared_ptr<Player> p_Player;
	std::shared_ptr<PlayerDodge> p_Dodge;
	std::shared_ptr<PlayerMove> p_Move;
	int _actionCount;
};