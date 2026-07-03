#pragma once
#include "../EnemyStateBase.h"
#include <memory>
class Actor;
class NormalEffect;
class Boss4StateDeath :
	public EnemyStateBase, public std::enable_shared_from_this<Boss4StateDeath>
{
public:
	Boss4StateDeath(std::weak_ptr<Actor> enemy, bool isWait);
	~Boss4StateDeath();
	void Init()override;
	void Update() override;
private:
};


