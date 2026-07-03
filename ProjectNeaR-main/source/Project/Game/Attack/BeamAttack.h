#pragma once
#include "CapsuleAttackBase.h"
#include <memory>
class AttackData;
class Actor;
class CharacterBase;
class BeamAttack :
    public CapsuleAttackBase
{
public:
    BeamAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner);
    ~BeamAttack();
    //XV
    void Update() override;
    //•`‰æ
    void Draw()const override;
    
    //‰ñ“]—Ê
    void SetRotaAngleAndAxis(float rotaAngle,Vector3 axis) { 
        m_rotaAngle = rotaAngle; 
        m_axis = axis;
    };
private:
    //‰ñ“]—Ê
    float m_rotaAngle;
    //‰ñ“]²
    Vector3 m_axis;
};

