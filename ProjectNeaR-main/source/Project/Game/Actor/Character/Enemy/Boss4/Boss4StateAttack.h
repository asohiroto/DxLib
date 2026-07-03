#pragma once
#include "../EnemyStateAttack.h"
class Actor;
class AttackData;
class AttackBase;
class EnemyBase;
class Boss4StateAttack :
	public EnemyStateAttack
{
public:
	Boss4StateAttack(std::weak_ptr<Actor> enemy, bool isWait, std::shared_ptr<AttackData> attackData);
	virtual ~Boss4StateAttack();
	void Init() override;
	void Update() override;
private:
	//UŒ‚ì¬
	void UpdateAttackFrame(std::shared_ptr<EnemyBase> owner)override;
	void CreateAttack(std::shared_ptr<EnemyBase> owner)override;
};

