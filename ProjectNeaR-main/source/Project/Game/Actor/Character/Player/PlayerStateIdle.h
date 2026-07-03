#pragma once
#include "PlayerStateBase.h"
class Actor;
class Player;
class PlayerStateIdle :
    public PlayerStateBase, public std::enable_shared_from_this<PlayerStateIdle>
{
public:
    PlayerStateIdle(std::weak_ptr<Actor>  player,bool isWait = false);
    ~PlayerStateIdle();
    void Init()override;
    void Update() override;
private:
    //アニメーション切り替え
    void ChangeAnim(std::shared_ptr<Player> owner);
};

