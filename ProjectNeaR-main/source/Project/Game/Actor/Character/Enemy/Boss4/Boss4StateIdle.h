#pragma once
#include "../EnemyStateBase.h"
class Actor;
class Boss4StateIdle :
    public EnemyStateBase, public std::enable_shared_from_this<Boss4StateIdle>
{
public:
    Boss4StateIdle(std::weak_ptr<Actor>  enemy, bool isWait = false);
    ~Boss4StateIdle();
    void Init()override;
    void Update() override;
};


