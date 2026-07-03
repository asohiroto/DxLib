#pragma once
#include "../EnemyStateBase.h"
class Actor;
class Boss3StateStart :
    public EnemyStateBase, public std::enable_shared_from_this<Boss3StateStart>
{
public:
    Boss3StateStart(std::weak_ptr<Actor>  enemy);
    ~Boss3StateStart();
    void Init()override;
    void Update() override;
private:
    //ƒJƒƒ‰¶¬‚ÆBGM•ÏX
    bool m_isCreate;
};


