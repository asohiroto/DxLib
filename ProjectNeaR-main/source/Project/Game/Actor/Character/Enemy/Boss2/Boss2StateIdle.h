#pragma once
#include "../EnemyStateBase.h"
class Actor;
class Boss2StateIdle :
    public EnemyStateBase, public std::enable_shared_from_this<Boss2StateIdle>
{
public:
    Boss2StateIdle(std::weak_ptr<Actor>  enemy, bool isWait = false);
    ~Boss2StateIdle();
    void Init()override;
    void Update() override;
};


