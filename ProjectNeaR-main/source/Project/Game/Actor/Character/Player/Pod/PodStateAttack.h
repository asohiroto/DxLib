#pragma once
#include "PodStateBase.h"
#include <memory>
class Actor;
class AttackData;
class TrackActorEffect;
class PodStateAttack :
    public PodStateBase, public std::enable_shared_from_this<PodStateAttack>
{
public:
    PodStateAttack(std::weak_ptr<Actor>  pod);
    ~PodStateAttack();
    void Init()override;
    void Update() override;
private:
    void CreateAttack(std::shared_ptr<Pod> owner, const Vector3& dir);
private:
    //攻撃データ
    std::shared_ptr<AttackData> m_attackData;
    //エフェクト
	std::weak_ptr<TrackActorEffect> m_shotEffect;
};

