#pragma once
#include "AttackBase.h"
class Actor;
class CharacterStateBase;
class SphereAttackBase abstract :
	public AttackBase
{
public:
	SphereAttackBase(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner);
	virtual ~SphereAttackBase() {};
	//XV
	virtual void Update()override;
	//’†“_
	void SetPos(const Vector3& pos);
	//”¼Œa
	void SetRadius(float radius);
};

