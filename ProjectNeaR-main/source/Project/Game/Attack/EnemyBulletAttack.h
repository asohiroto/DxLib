#pragma once
#include "BulletAttack.h"
#include <memory>
class AttackData;
class Collidable;
class NormalEffect;
class CharacterBase;
class EnemyBulletAttack :
    public BulletAttack
{
public:
    EnemyBulletAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner);
    ~EnemyBulletAttack();

    //•`‰æ
    void Draw()const override;
    //“–‚½‚Á‚½
    void OnCollide(const std::shared_ptr<Collidable> other) override;

    //UŒ‚‚Å‰ó‚ê‚é‚©
    void SetIsDestructible(bool isDestructible) { m_isDestructible = isDestructible; };
    bool IsDestructible()const { return m_isDestructible; };
private:
    //UŒ‚‚É“–‚½‚Á‚½‚É‰ó‚ê‚é‚©
    bool m_isDestructible;
private:
};

