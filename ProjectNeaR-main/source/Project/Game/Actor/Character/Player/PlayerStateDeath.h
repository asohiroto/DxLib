#pragma once
#include "PlayerStateBase.h"
class Actor;
class PlayerStateDeath :
    public PlayerStateBase, public std::enable_shared_from_this<PlayerStateDeath>
{
public:
    PlayerStateDeath(std::weak_ptr<Actor>  player, bool isWait);
    ~PlayerStateDeath();
    void Init()override;
    void Update() override;
private:
};

