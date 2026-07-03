#pragma once
#include "EnemyStateBase.h"
class Actor;
class EnemyStateFall :
	public EnemyStateBase, public std::enable_shared_from_this<EnemyStateFall>
{
public:
	EnemyStateFall(std::weak_ptr<Actor> enemy, bool isWait,bool isHit = false);
	~EnemyStateFall();
	void Init()override;
	void Update() override;
private:
	bool m_isHit;
};


