#pragma once
#include "../EnemyStateBase.h"
class Actor;
class Boss4StateRandMoving :
    public EnemyStateBase, public std::enable_shared_from_this<Boss4StateRandMoving>
{
public:
    Boss4StateRandMoving(std::weak_ptr<Actor>  enemy, bool isWait);
    ~Boss4StateRandMoving();
    void Init()override;
    void Update() override;
private:
    //ˆÚ“®•ûŒü
    Vector3 m_randVec;
};


