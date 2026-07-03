#pragma once
#include "../EnemyStateBase.h"
class Actor;
class EnemyBase;
class Boss3StateHit :
	public EnemyStateBase, public std::enable_shared_from_this<Boss3StateHit>
{
public:
	Boss3StateHit(std::weak_ptr<Actor> enemy, bool isWait);
	~Boss3StateHit();
	void Init()override;
	void Update() override;

};

