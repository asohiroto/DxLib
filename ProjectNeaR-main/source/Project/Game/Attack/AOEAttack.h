#pragma once
#include "SphereAttackBase.h"
class AOEAttack :
    public SphereAttackBase
{
public:
    AOEAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner);
    ~AOEAttack();
	//XV
	void Update() override;
    //•`‰æ
    void Draw()const override;
    //“–‚½‚Á‚½
    void OnCollide(const std::shared_ptr<Collidable> other) override;
};

