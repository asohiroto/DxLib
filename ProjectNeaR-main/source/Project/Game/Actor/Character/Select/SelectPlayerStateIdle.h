#pragma once
#include "../CharacterStateBase.h"
class SelectPlayerStateIdle :
    public CharacterStateBase, public std::enable_shared_from_this<SelectPlayerStateIdle>
{
public:
    SelectPlayerStateIdle(std::weak_ptr<Actor>  player);
    ~SelectPlayerStateIdle();
    void Init()override;
    void Update() override;
private:
};
