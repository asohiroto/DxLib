#pragma once
#include "AttackBase.h"
#include "../../General/Math/MyMath.h"
class Actor;
class CharacterStateBase;
class AttackData;
class CapsuleAttackBase abstract :
	public AttackBase
{
public:
	CapsuleAttackBase(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner);
	virtual ~CapsuleAttackBase() {};
	//更新
	virtual void Update()override;
	//始点
	void SetStartPos(const Vector3& pos);
	//終点
	void SetEndPos(const Vector3& pos);
	//半径
	void SetRadius(float radius);
protected:
};

