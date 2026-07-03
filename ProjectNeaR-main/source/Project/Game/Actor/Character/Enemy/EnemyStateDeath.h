#pragma once
#include "EnemyStateBase.h"
class Actor;
class EnemyStateDeath :
	public EnemyStateBase, public std::enable_shared_from_this<EnemyStateDeath>
{
public:
	EnemyStateDeath(std::weak_ptr<Actor> enemy, bool isWait);
	~EnemyStateDeath();
	void Init()override;
	void Update() override;
};


