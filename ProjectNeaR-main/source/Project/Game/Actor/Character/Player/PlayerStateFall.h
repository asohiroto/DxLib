#pragma once
#include "PlayerStateBase.h"
class Actor;
class CharaStatus;
class Player;
class PlayerStateFall :
    public PlayerStateBase, public std::enable_shared_from_this<PlayerStateFall>
{
public:
    PlayerStateFall(std::weak_ptr<Actor>  player, bool isWait);
    ~PlayerStateFall();
    void Init()override;
    void Update() override;
    //ŠŠ‹óó‘Ô‚©
    bool IsGliding()const { return m_isGliding; };
private:
    //ŠŠ‹óó‘Ô‚©
    bool m_isGliding;
    //ŠŠ‹óŒã
    bool m_isGlided;
private:
    //—‰º’†‚ÌˆÚ“®
    void MoveFall(Input& input, std::shared_ptr<Player> owner, std::shared_ptr<CharaStatus> status);
};

