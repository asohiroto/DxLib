#pragma once
#include "../CharacterStateBase.h"
#include <memory>
#include "../../../../General/Math/MyMath.h"
#include <string>
class Actor;
class EnemyStateBase abstract :
	public CharacterStateBase
{
public:
	EnemyStateBase(std::weak_ptr<Actor> enemy, bool isWait);
	virtual ~EnemyStateBase();


};

