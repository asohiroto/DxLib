#pragma once
#include "PlayerStateBase.h"
class Actor;
class PlayerStateStart :
    public PlayerStateBase, public std::enable_shared_from_this<PlayerStateStart>
{
public:
    PlayerStateStart(std::weak_ptr<Actor>  player);
    ~PlayerStateStart();
    void Init()override;
    void Update() override;
private:
};

