#pragma once
#include "CapsuleAttackBase.h"
#include <memory>
class AttackData;
class Actor;
class CharacterBase;
class SwordAttack :
    public CapsuleAttackBase
{
public:
    SwordAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner);
    ~SwordAttack();
	//çXêV
	void Update() override;
    //ï`âÊ
    void Draw()const override;
};

