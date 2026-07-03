#pragma once
#include "EnemyStateBase.h"
class Actor;
class EnemyStateDown :
	public EnemyStateBase, public std::enable_shared_from_this<EnemyStateDown>
{
public:
	EnemyStateDown(std::weak_ptr<Actor> enemy, bool isWait);
	~EnemyStateDown();
	void Init()override;
	void Update() override;
};


