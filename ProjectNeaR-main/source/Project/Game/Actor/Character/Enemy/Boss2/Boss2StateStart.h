#pragma once
#include "../EnemyStateBase.h"
class Actor;
class Boss2StateStart :
    public EnemyStateBase, public std::enable_shared_from_this<Boss2StateStart>
{
public:
    Boss2StateStart(std::weak_ptr<Actor>  enemy);
    ~Boss2StateStart();
    void Init()override;
    void Update() override;
private:
    //ƒJƒƒ‰‚ÆBGM‚Ì•ÏX
    bool m_isCreate;
};


