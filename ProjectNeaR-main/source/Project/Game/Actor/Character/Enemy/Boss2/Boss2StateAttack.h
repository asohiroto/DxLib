#pragma once
#include "../EnemyStateAttack.h"
class Actor;
class AttackData;
class AttackBase;
class EnemyBase;
class Boss2StateAttack :
    public EnemyStateAttack
{
public:
	Boss2StateAttack(std::weak_ptr<Actor> enemy, bool isWait, std::shared_ptr<AttackData> attackData);
	virtual ~Boss2StateAttack();
	void Init() override;
	void Update() override;
	//UŒ‚ì¬
	void CreateAttack(std::shared_ptr<EnemyBase> owner)override;
};

