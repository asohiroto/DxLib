#pragma once
#include "EnemyStateBase.h"
class Actor;
class EnemyStateIdle :
    public EnemyStateBase, public std::enable_shared_from_this<EnemyStateIdle>
{
public:
    EnemyStateIdle(std::weak_ptr<Actor>  enemy,bool isWait = false);
    ~EnemyStateIdle();
    void Init()override;
    void Update() override;
};

