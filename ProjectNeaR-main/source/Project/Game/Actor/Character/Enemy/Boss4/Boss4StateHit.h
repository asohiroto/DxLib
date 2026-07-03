#pragma once
#include "../EnemyStateBase.h"
class Actor;
class EnemyBase;
class Boss4StateHit :
	public EnemyStateBase, public std::enable_shared_from_this<Boss4StateHit>
{
public:
	Boss4StateHit(std::weak_ptr<Actor> enemy, bool isWait);
	~Boss4StateHit();
	void Init()override;
	void Update() override;
};

