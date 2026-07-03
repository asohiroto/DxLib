#include "EnemyStateBase.h"

EnemyStateBase::EnemyStateBase(std::weak_ptr<Actor> enemy, bool isWait):
	CharacterStateBase(enemy,isWait)
{
}

EnemyStateBase::~EnemyStateBase()
{
}
