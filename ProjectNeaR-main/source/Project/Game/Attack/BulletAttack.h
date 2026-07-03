#pragma once
#include "SphereAttackBase.h"
#include <memory>
class AttackData;
class Collidable;
class CharacterBase;
class NormalEffect;
class BulletAttack :
    public SphereAttackBase
{
public:
    BulletAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner);
    ~BulletAttack();
 
    //更新
    virtual void Update() override;
    //描画
    virtual void Draw()const override;
    //当たった時
    virtual void OnCollide(const std::shared_ptr<Collidable> other) override;

    //リセット
    void Reset(float keepFrame);

    //移動ベクトル
    void SetMoveVec(Vector3 vec) { m_moveVec = vec; };

    //アクティブ
    void SetActive(bool isActive) { m_isActive = isActive; };
    bool IsActive()const { return m_isActive; };
protected:
    //移動
    Vector3 m_moveVec;

    //アクティブ(弾の更新と描画を行うか)
    bool m_isActive;

protected:
    //移動
    virtual void UpdateMove();
};

