#pragma once
#include "BulletAttack.h"
#include <memory>
class CharacterBase;
class EnemyBase;
class MissileAttack :
    public BulletAttack
{
public:
    MissileAttack(std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner);
    ~MissileAttack();
    //描画
    void Draw()const override;

    //ターゲットを追跡するための変数をセット
    void SetTargetTracking(std::weak_ptr<CharacterBase> pTarget, float trackingRate, float trackingFrame);
private:
    //ターゲット
    std::weak_ptr<CharacterBase> m_pTarget;
    //追跡力
    float m_trackingRate;
    //追跡フレーム
    float m_trackingFrame;
    //追尾移動
    Vector3 m_trackingVec;
private:
    //攻撃作成
    void CreateAttack(std::shared_ptr<EnemyBase> owner);
    //移動
    void UpdateMove() override;
};

