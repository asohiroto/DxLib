#pragma once
#include "PlayerStateBase.h"
class Actor;
class Player;
class CharaStatus;
class PlayerStateJump :
    public PlayerStateBase, public std::enable_shared_from_this<PlayerStateJump>
{
public:
    PlayerStateJump(std::weak_ptr<Actor> player, bool isWait);
    ~PlayerStateJump();
    void Init()override;
    void Update() override;
private:
    //ƒWƒƒƒ“ƒv’†‚ÌˆÚ“®
    void MoveJump(Input& input, std::shared_ptr<Player> owner, std::shared_ptr<CharaStatus> status);
};

