#pragma once
#include "../EnemyStateBase.h"
class Actor;
class Boss4StateStart :
    public EnemyStateBase, public std::enable_shared_from_this<Boss4StateStart>
{
public:
    Boss4StateStart(std::weak_ptr<Actor>  enemy);
    ~Boss4StateStart();
    void Init()override;
    void Update() override;
private:
    //ƒJƒƒ‰¶¬
    bool m_isCreate;
};


