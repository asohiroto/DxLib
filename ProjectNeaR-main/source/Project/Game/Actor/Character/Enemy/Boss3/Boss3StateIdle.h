#pragma once
#include "../EnemyStateBase.h"
class Actor;
class Boss3StateIdle :
    public EnemyStateBase, public std::enable_shared_from_this<Boss3StateIdle>
{
public:
    Boss3StateIdle(std::weak_ptr<Actor>  enemy, bool isWait = false);
    ~Boss3StateIdle();
    void Init()override;
    void Update() override;
};


